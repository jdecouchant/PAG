//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 
cplusplus {{	
        #include "base.h"
}}

packet HistoryPacket {
 		int8_t msgType;
 		int senderId;
 		int receiverId;
 		// + roundId;
 		
 		int updatesId[]; // contains the updates id owned by sender, the old list in the PUSH protocol
 		// + hashed buffermap of last round
 		// + signature of the whole message
}

packet BriefPacket {
 		int8_t msgType;
 		int senderId;
 		// + roundId
 		
 		int updatesId[]; // contains the updates id sent by sender
 		// + encrypted updates
 		// + signature of the whole message   
}


