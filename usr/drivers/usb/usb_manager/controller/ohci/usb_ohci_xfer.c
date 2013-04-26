/*
 * Copyright (c) 2007-2013 ETH Zurich.
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached LICENSE file.
 * If you do not find this file, copies can be found by writing to:
 * ETH Zurich D-INFK, Haldeneggsteig 4, CH-8092 Zurich. Attn: Systems Group.
 */

#include "usb_ohci_descriptors.h"
#include "usb_ohci_xfer.h"
#include "../usb_endpoint.h"

static void usb_ohci_xfer_short_frames(struct usb_xfer *xfer)
{
    usb_ohci_td_t *td;
    usb_ohci_ed_t *ed;

    usb_ohci_td_ctrl_t *td_ctrl;
    uint16_t cc;
    usb_paddr_t td_next;
    usb_paddr_t current_buffer;

    td = xfer->hcd_td_cache;

    /*
     * loop over the frame, a frame may contain more than one short
     * packets, we have to make sure that we reached the last one
     */
    while (1) {
        /* TODO: invalidate chache ? */
        current_buffer = td->td_current_buffer;
        td_ctrl = &td->td_control;
        td_next = td->td_nextTD;

        /*
         * check if we have reached the last transfer descriptor
         * if so we are done
         */

        if (((void *) td) == xfer->hcd_td_last) {
            td = NULL;
            break;
        }

        /*
         * check the condition codes, if it is USB_OHCI_STATUS_OK then
         * the transfer is finished
         */
        cc = td_ctrl->condition_code;
        if (cc) {
            td = NULL;
            break;
        }

        /*
         * check if we have reached the last packet i.e. the td_nextTD is
         * NULL, but hwe have to mask out the last four bits, since these may
         * be used otherwise.
         * If we have a current buffer then there is something else in the
         * frame we follow the alternative and stop processing.
         */
        if (((td_next & (~0xF)) == 0) || current_buffer) {
            td = td->alt_next;
            break;
        }

        // go to next transfer descriptor
        td = td->obj_next;
    }

    // update of the cache
    xfer->hcd_td_cache = td;

    /*
     * we have found a non completed short transfer for this endpoint
     * this means we have to update the head pointer of the endpoint
     * descriptor to this one
     */
    if (td) {
        // get the associated endpoint
        ed = xfer->hcd_qh_start[xfer->flags_internal.curr_dma_set];

        ed->ed_headP = td->td_self;

        // TODO: invalideate cache?

        /*
         * we need to make sure that the OHCI takes up this remaining
         * transfer descriptor for processing.
         */
        if (xfer->type == USB_XFER_TYPE_BULK) {
            /* TODO: write register BLF
             * OWRITE4(sc, OHCI_COMMAND_STATUS, OHCI_BLF);
             */
        }

        if (xfer->type == USB_XFER_TYPE_CTRL) {
            /* TODO: write register CLF
             * OWRITE4(sc, OHCI_COMMAND_STATUS, OHCI_CLF);
             */
        }
    }
}

/**
 * \brief   this function checks if a USB transfer is already finished or not
 *
 * \param   xfer    the usb transfer to check for completition
 *
 * \return  0       if the usb transfer is not finished
 *          else    if the usb transfer is finished
 */
uint8_t usb_ohci_xfer_is_finished(struct usb_xfer *xfer)
{
    usb_ohci_ed_t *ed;
    usb_paddr_t ed_headP;
    usb_paddr_t ed_tailP;

    // getting the endpoint from the queue head list
    ed = xfer->hcd_qh_start[xfer->flags_internal.curr_dma_set];

    /* TODO: invalidate cache ? */

    // get the transfer descriptor pointers
    ed_headP = ed->ed_headP;
    ed_tailP = ed->ed_tailP;

    /*
     *  if the endpoint is halted or there are no transfer descriptors
     *  then there is no activitiy
     */
    if (USB_OHCI_EP_HALTED(ed) || !USB_OHCI_EP_HAS_TD(ed_headP, ed_tailP)) {
        if (xfer->type == USB_XFER_TYPE_ISOC) {
            // isochronus endponts have to be treated differently;
            usb_ohci_xfer_done_isoc(xfer);

            // transfer completed
            return 1;
        }

        if (xfer->flags_internal.short_frames_ok) {
            usb_ohci_xfer_short_frames(xfer);

            if (xfer->hcd_td_cache) {
                return 0;
            }
        }

        // handle the data toggle flag
        if (USB_OHCI_EP_CARRY(ed)) {
            xfer->endpoint->data_toggle = 1;
        } else {
            xfer->endpoint->data_toggle = 0;
        }

        // handle the completition of the xfer
        usb_ohci_xfer_done(xfer);

        // transfers completed
        return 1;
    }

    // transfer is not completed yet
    return 0;
}

/**
 * \brief This function updates the frame_lengths of the usb transfer
 *
 * \param xfer  the current USB transfer
 *
 * \return USB_ERR_OK on success
 *         USB_ERR_IO
 *         USB_ERR_STALLED
 */
static usb_error_t usb_ohci_xfer_update_frame_lengths(struct usb_xfer *xfer)
{
    usb_ohci_td_t *td;
    usb_ohci_td_t *td_alt_next;
    uint32_t temp;
    usb_paddr_t phy_start;
    usb_paddr_t phy_end;
    usb_ohci_td_ctrl_t td_flags;
    uint16_t cc;

    td = xfer->hcd_td_cache;
    td_alt_next = td->alt_next;

    td_flags = 0;

    if (xfer->actual_frames != xfer->num_frames) {
        if (xfer->actual_frames < xfer->max_frame_count) {
            xfer->frame_lengths[xfer->actual_frames] = 0;
        } else {
            // TODO: error handling if actual frame is bigger than max frames
            assert(!"Frame overflow");
        }
    }

    while (1) {
        phy_start = td->td_current_buffer;
        td_flags = td->td_control;
        cc = td_flags.condition_code;

        if (phy_start) {
            /*
             * the current buffer pointer is non zero, this means we
             * have some data in the buffer, but buffer is not full.
             * i.e. we have to deal with short transfers
             */
            phy_end = td->td_buffer_end;

            /*
             * calculate the remaining bytes in the buffer.
             * Basically end-start + 1
             */
            temp = (USB_OHCI_PAGE(phy_start ^ phy_end) ?
                    (USB_OHCI_PAGE_SIZE + 1) : 0x0001);
            temp += USB_OHCI_PAGE_OFFSET(phy_end);
            temp -= USB_OHCI_PAGE_OFFSET(phy_start);

            /*
             * we may have more data in the buffer left as the transfer
             * size indicates. That case we stall the transfer.
             */
            if (temp > td->len) {
                cc = USB_OHCI_STATUS_STALL;
            } else if (xfer->actual_frames != xfer->num_frames) {
                /*
                 * set the frame_length of the actual frame
                 */
                xfer->frame_lengths[xfer->actual_frames] += td->len - temp;
            }
        } else {
            /*
             * the transfer was complete one frame before, so we
             * set the frame length of the actual frame to be the
             * total transfer length
             */
            if (xfer->actual_frames != xfer->num_frames) {
                xfer->frame_lengths[xfer->actual_frames] += td->len;
            }
        }

        /*
         * if this TD was the last of USB the transfer, we are done and
         * can go out of the loop (tranfer finished).
         */
        if (((void *) td) == xfer->hcd_td_last) {
            td = NULL;
            break;
        }

        /*
         * if we have a condition flag other than USB_OHCI_STATUS_OK then
         * we treat this as completed and go out of the loop.
         * (transfer finished).
         */
        if (cc) {
            td = NULL;
            break;
        }

        /*
         * we encountered a short transfer, we have to check now if
         * short transfers are acceptable or not by checking the
         * short_frames_ok flag. If this is the case, we follow the
         * alternative next pointer to the TD filling up the remainder of
         * this frame. If short frames are not acceptable this means
         * we have reached the end of the transfer
         */
        if (phy_start) {
            if (xfer->flags_internal.short_frames_ok) {
                td = td->alt_next;
            } else {
                td = NULL;
            }
            break;
        }

        // take the next TD in the list
        td = td->obj_next;

        /*
         * we have a new alternative next pointer this belongs
         * to another transfer thus this transfer is complete
         */
        if (td->alt_next != td_alt_next) {
            break;
        }
    }

    /* update transfer cache */

    xfer->hcd_td_cache = td;

    return ((cc == USB_OHCI_STATUS_OK) ? USB_ERR_OK :
            (cc == USB_OHCI_STATUS_STALL) ? USB_ERR_STALLED : USB_ERR_IOERROR);
}

/**
 * \brief   this function handles the completion of a transfer
 *
 * \param   xfer    the usb transfer to handle completion
 */
void usb_ohci_xfer_done(struct usb_xfer *xfer)
{
    usb_error_t err = USB_ERR_OK;

    /*
     * go over the td list and handle the competition
     */
    xfer->hcd_td_cache = xfer->hcd_td_first;

    /*
     * check if it is a control transfer. If this is the case
     * we may need to do some extra work, because control headers
     * could be requested
     */
    if (xfer->flags_internal.ctrl_xfer) {
        if (xfer->flags_internal.ctrl_header) {
            err = usb_ohci_xfer_update_frame_lengths(xfer);
        }
        xfer->actual_frames = 1;

        if (xfer->hcd_td_cache == NULL) {
            usb_ohci_xfer_remove(xfer, err);
            return;
        }
    }

    /*
     * process the remaining frames till all is complete
     */
    while (xfer->actual_frames < xfer->num_frames) {
        err = usb_ohci_xfer_update_frame_lengths(xfer);
        xfer->actual_frames++;

        if (xfer->hcd_td_cache == NULL) {
            usb_ohci_xfer_remove(xfer, err);
            return;
        }
    }

    /*
     * check if there is a control transfer active at the moment
     */
    if (xfer->flags_internal.ctrl_xfer && !xfer->flags_internal.ctrl_active) {
        err = usb_ohci_xfer_update_frame_lengths(xfer);
    }

    usb_ohci_xfer_remove(xfer, err);

}

/**
 * \brief
 *
 * \param   xfer    the usb transfer to check for completion
 *
 * \return  0       if the usb transfer is not finished
 *          else    if the usb transfer is finished
 */
void usb_ohci_xfer_done_isoc(struct usb_xfer *xfer)
{
    assert(!"NYI: need to check isochronus transfer competition");
}

/**
 * \brief This function is called by the specific pipe functions when a
 *        usb transfer is finished. The transfer descriptors are removed
 *        from the endpoint lists.
 *
 * \param xfer  the usb transfer request that is finished
 * \param error status code of the finished transfer request
 */
void usb_ohci_xfer_remove(struct usb_xfer *xfer, usb_error_t error)
{
    usb_ohci_ed_t *ed;

    // get the host controller
    usb_ohci_hc_t *hc = (usb_ohci_hc_t *) xfer->host_controller->hc_control;

    // get the endpoint associated with the usb transfer
    ed = xfer->hcd_qh_start[xfer->flags_internal.curr_dma_set];

    // TODO: invalidate page cache of endpoint

    switch (xfer->type) {
        case USB_XFER_TYPE_ISOC:
            usb_ohci_remove_qh(ed, hc->qh_isoc_last);
            break;
        case USB_XFER_TYPE_INTR:
            usb_ohci_remove_qh(ed, hc->qh_intr_last[xfer->intr_qh_pos]);
            break;
        case USB_XFER_TYPE_CTRL:
            usb_ohci_remove_qh(ed, hc->qh_ctrl_last);
            break;
        case USB_XFER_TYPE_BULK:
            usb_ohci_remove_qh(ed, hc->qh_bulk_last);
            break;
        default:
            assert(!"Invalid Transfer Type");
            break;
    }

    // set the transfer descriptor pointers to null
    xfer->hcd_td_first = NULL;
    xfer->hcd_td_last = NULL;

    /*
     * remove from interrupt queue and enqueue to done queue
     */
    usb_xfer_done(xfer, error);
}

/*
 * \brief enqueues the transfer on the controller's interrupt queue to
 *        handle the completed transfers
 *
 * \param xfer  the transfer to enqueue
 */
void usb_ohci_xfer_enqueue(struct usb_xfer *xfer)
{
    /* check for early completion */
    if (usb_ohci_xfer_is_finished(xfer)) {
        return;
    }
    /* put transfer on interrupt queue */
    usb_xfer_enqueue(xfer->host_controller->intr_queue, xfer);

    /* start timeout, if any */
    /* TODO: handle time out
     if (xfer->timeout != 0) {
     usbd_transfer_timeout_ms(xfer, &ohci_timeout, xfer->timeout);
     }*/
}

struct usb_ohci_setup_td {
    struct usb_page_cache *pc;
    usb_ohci_td_t *td;
    usb_ohci_td_t *td_next;
    uint32_t average;
    usb_ohci_td_ctrl_t td_flags;
    uint32_t len;
    uint16_t max_frame_size;
    uint8_t shortpkt;
    uint8_t setup_alt_next;
    uint8_t last_frame;
};

/**
 * \brief   this function is used to setup the transfer descriptors
 *          and allocating buffers for the transfers
 *
 * \param temp  setup information for the transfer descriptors
 */
static void usb_ohci_xfer_setup_td(struct usb_ohci_setup_td *temp)
{
    usb_ohci_td_t *td;
    usb_ohci_td_t *td_next;
    usb_ohci_td_t *td_alt_next = NULL;

    uint32_t average = 0;
    uint8_t old_shortpkt = temp->shortpkt;
    uint32_t old_len = temp->len;

    /*
     * this is used to precompute the length of the transfer
     */
    uint8_t precompute = 1;
    uint8_t restart = 1;

    /*
     * we are using software to detect short packets thus we allow
     * buffer rounding i.e. the buffer does not have to be filled
     * completely
     */
    if (temp->td_flags.direction_pid == USB_OHCI_PID_IN) {
        temp->td_flags.rounding = 1;
    } else {
        temp->td_flags.rounding = 0;
    }

    td = temp->td;
    td_next = temp->td_next;

    while (restart) {

        while (1) {
            if (temp->len == 0) {
                /*
                 * handling of zero length packets. these packets are sent last
                 * thus we stop processing if we see the short packet flag
                 */
                if (temp->shortpkt) {
                    break;
                }

                temp->shortpkt = 1;
                average = 0;

            } else {
                average = temp->average;

                /*
                 * check if the length of the transfer is smaller than the
                 * average i.e. if we have a short packet not using the whole
                 * USB frame
                 */
                if (temp->len < average) {
                    if (temp->len % temp->max_frame_size) {
                        temp->shortpkt = 1;
                    }
                    average = temp->len;
                }
            }

            if (td_next == NULL) {
                /*
                 * TODO: PANIC: not enough transport descriptors
                 */
                assert(!"ran out of transfer descriptors!");
            }

            /*
             * get the next td to process
             */
            td = td_next;
            td_next = td->obj_next;

            /*
             * check if we are pre-computing the lengths.
             * update the remaining length and continue with the loop
             */
            if (precompute) {
                temp->len -= average;

                continue;
            }

            /* fill in the current TD with the data */
            td->td_control = temp->td_flags;

            /* the next td uses the toggle carry */
            temp->td_flags.data_toggle = 1;

            /*
             * setup the fields in the transfer descriptor depending on the
             * packet size, zero length packets do not need a buffer
             * buffer but other wise we need
             */
            if (average == 0) {
                td->td_current_buffer = 0;
                td->td_buffer_end = 0;
                td->len = 0;
            } else {
                // buffer size is bigger than the td internal buffer
                if (average > USB_OHCI_TD_BUFFER_SIZE) {
                    /*
                     * TODO: allocate a bigger buffer here
                     */
                    assert(!"NYI: handling of biger buffers ");
                } else {
                    td->td_current_buffer = td->td_self
                            + USB_OHCI_TD_BUFFER_OFFSET;
                    td->td_buffer_end = td->td_current_buffer + average;
                }

                /* set the length of this transfer descriptor */
                td->len = average;

                /* update the remaining length of the transfer */
                temp->len -= average;
            }

            if ((td_next == td_alt_next) && temp->setup_alt_next) {
                /*
                 * we have to setup the alternative transfer descriptors
                 * because we need to receive these frames one by one
                 *
                 * - delay interrupts must be 1
                 * - last td in the list
                 */
                td->td_control.delay_interrupt = 1;
                td->td_nextTD = 0;

            } else {
                /*
                 * do the linking of the transfer descriptors using
                 * their physical addresses if there is another
                 * td in the list
                 */
                if (td_next) {
                    td->td_nextTD = td_next->td_self;
                }
            }

            td->alt_next = td_alt_next;
        }

        if (precompute) {
            precompute = 0;

            /* the last frame does not have an alternative next td */
            if (temp->last_frame) {
                td_alt_next = NULL;
            } else {
                td_alt_next = td_next;
            }

            /*
             * we were precomputing so we need to restore the values
             */
            temp->shortpkt = old_shortpkt;
            temp->len = old_len;
        } else {
            restart = 0;
        }

    }

    temp->td = td;
    temp->td_next = td_next;
}


/**
 * \brief   this function sets up the standard chains for the  transfer
 *          descriptors for an USB transfer.
 *
 * \param xfer      the usb transfer to setup the transfer descriptors
 * \param ed_last   the last endpoint descriptor
 */
void usb_ohci_xfer_setup(struct usb_xfer *xfer, usb_ohci_ed_t **ed_last)
{

}
