% BEGIN LICENSE BLOCK
% Version: CMPL 1.1
%
% The contents of this file are subject to the Cisco-style Mozilla Public
% License Version 1.1 (the "License"); you may not use this file except
% in compliance with the License.  You may obtain a copy of the License
% at www.eclipse-clp.org/license.
% 
% Software distributed under the License is distributed on an "AS IS"
% basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See
% the License for the specific language governing rights and limitations
% under the License. 
% 
% The Original Code is  CPViz Constraint Visualization System
% The Initial Developer of the Original Code is  Helmut Simonis
% Portions created by the Initial Developer are
% Copyright (C) 2009-2010 Helmut Simonis
% 
% Contributor(s): 	Helmut Simonis, 4C, Univerity College Cork, Cork
%			
% 
% END LICENSE BLOCK
% ----------------------------------------------------------------------
:-module(data).

:-export(network_topology/3).

network_topology(mci,19,[e(1,2,1),
                         e(1,5,1),
                         e(1,6,1),
                         e(2,3,1),
                         e(2,5,1),
                         e(2,12,1),
                         e(3,4,1),
                         e(4,5,1),
                         e(4,8,1),
                         e(4,10,1),
                         e(5,6,1),
                         e(6,11,1),
                         e(6,12,1),
                         e(6,18,1),
                         e(7,8,1),
                         e(7,9,1),
                         e(8,10,1),
                         e(8,11,1),
                         e(8,12,1),
                         e(9,10,1),
                         e(10,17,1),
                         e(10,19,1),
                         e(11,12,1),
                         e(12,13,1),
                         e(12,18,1),
                         e(13,14,1),
                         e(14,18,1),
                         e(15,18,1),
                         e(16,17,1),
                         e(16,18,1),
                         e(17,18,1),
                         e(17,19,1)]).
                         
network_topology(eon,20,[e(1, 2,1),
                         e(1, 5,1),
                         e(2, 4,1),
                         e(3, 4,1),
                         e(3, 5,1),
                         e(4, 5,1),
                         e(4, 6,1),
                         e(4, 9,1),
                         e(4, 10,1),
                         e(5, 6,1),
                         e(5, 7,1),
                         e(5, 14,1),
                         e(5, 15,1),
                         e(6, 7,1),
                         e(6, 8,1),
                         e(6, 10,1),
                         e(7, 8,1),
                         e(7, 12,1),
                         e(7, 13,1),
                         e(7, 15,1),
                         e(9, 10,1),
                         e(9, 11,1),
                         e(9, 12,1),
                         e(9, 15,1),
                         e(9, 17,1),
                         e(10, 12,1),
                         e(11, 17,1),
                         e(11, 19,1),
                         e(12, 15,1),
                         e(12, 16,1),
                         e(13, 14,1),
                         e(13, 18,1),
                         e(14, 18,1),
                         e(15, 16,1),
                         e(15, 18,1),
                         e(15, 20,1),
                         e(16, 17,1),
                         e(17, 19,1),
                         e(18, 20,1)]).


network_topology(brezil,27,[e(1,2,1),
                            e(2,3,1),
                            e(3,4,1),
                            e(4,5,1),
                            e(5,6,1),
                            e(6,19,1),
                            e(19,20,1),
                            e(20,21,1),
                            e(21,22,1),
                            e(22,23,1),
                            e(23,24,1),
                            e(24,25,1),
                            e(25,26,1),
                            e(26,27,1),
                            e(16,27,1),
                            e(16,17,1),
                            e(15,26,1),
                            e(15,18,1),
                            e(13,15,1),
                            e(13,14,1),
                            e(14,18,1),
                            e(15,17,1),
                            e(11,13,1),
                            e(8,11,1),
                            e(1,8,1),
                            e(11,17,1),
                            e(22,24,1),
                            e(21,25,1),
                            e(20,25,1),
                            e(19,25,1),
                            e(19,26,1),
                            e(24,26,1),
                            e(20,26,1),
                            e(12,23,1),
                            e(12,17,1),
                            e(10,12,1),
                            e(4,11,1),
                            e(7,10,1),
                            e(5,7,1),
                            e(6,7,1),
                            e(10,19,1),
                            e(12,19,1),
                            e(12,27,1),
                            e(4,7,1),
                            e(4,10,1),
                            e(6,12,1),
                            e(4,9,1),
                            e(1,3,1),
                            e(3,8,1),
                            e(4,8,1),
                            e(13,17,1),
                            e(12,13,1),
                            e(9,11,1),
                            e(10,15,1),
                            e(9,15,1),
                            e(11,16,1),
                            e(7,26,1),
                            e(19,24,1),
                            e(25,27,1),
                            e(17,18,1),
                            e(11,12,1),
                            e(13,26,1),
                            e(10,24,1),
                            e(13,18,1),
                            e(12,18,1),
                            e(8,9,1),
                            e(11,15,1),
                            e(9,10,1),
                            e(6,26,1),
                            e(12,16,1)]).

network_topology(nsf,14,[e(1,2,1),
                         e(1,3,1),
                         e(1,8,1),
                         e(2,3,1),
                         e(2,4,1),
                         e(3,6,1),
                         e(4,5,1),
                         e(4,11,1),
                         e(5,6,1),
                         e(5,7,1),
                         e(6,10,1),
                         e(6,14,1),
                         e(7,8,1),
                         e(8,9,1),
                         e(9,10,1),
                         e(9,12,1),
                         e(9,13,1),
                         e(11,12,1),
                         e(11,13,1),
                         e(12,14,1),
                         e(13,14,1)]).


network_topology(eon2,19,[e(1,2,1),
                          e(1,2,1),
                          e(1,5,1),
                          e(1,5,1),
                          e(2,4,1),
                          e(2,4,1),
                          e(3,4,1),
                          e(3,4,1),
                          e(3,5,1),
                          e(3,5,1),
                          e(4,10,1),
                          e(4,10,1),
                          e(4,5,1),
                          e(4,5,1),
                          e(4,6,1),
                          e(4,6,1),
                          e(4,9,1),
                          e(4,9,1),
                          e(5,14,1),
                          e(5,14,1),
                          e(5,15,1),
                          e(5,15,1),
                          e(5,6,1),
                          e(5,6,1),
                          e(5,7,1),
                          e(5,7,1),
                          e(6,10,1),
                          e(6,10,1),
                          e(6,7,1),
                          e(6,7,1),
                          e(6,8,1),
                          e(6,8,1),
                          e(7,12,1),
                          e(7,12,1),
                          e(7,13,1),
                          e(7,13,1),
                          e(7,15,1),
                          e(7,15,1),
                          e(7,8,1),
                          e(7,8,1),
                          e(8,10,1),
                          e(8,10,1),
                          e(8,12,1),
                          e(8,12,1),
                          e(9,10,1),
                          e(9,10,1),
                          e(9,11,1),
                          e(9,11,1),
                          e(9,12,1),
                          e(9,12,1),
                          e(9,15,1),
                          e(9,15,1),
                          e(9,17,1),
                          e(9,17,1),
                          e(10,12,1),
                          e(10,12,1),
                          e(11,17,1),
                          e(11,17,1),
                          e(12,15,1),
                          e(12,15,1),
                          e(12,16,1),
                          e(12,16,1),
                          e(13,14,1),
                          e(13,14,1),
                          e(13,18,1),
                          e(13,18,1),
                          e(14,18,1),
                          e(14,18,1),
                          e(15,16,1),
                          e(15,16,1),
                          e(15,18,1),
                          e(15,18,1),
                          e(15,19,1),
                          e(15,19,1),
                          e(16,17,1),
                          e(16,17,1),
                          e(18,19,1),
                          e(18,19,1)]).
