

/* LEGACY CODE - LB
			uint32_t byte_offset = (reg[rs1]+imm) % 4;
			uint32_t load_mask;
			//Calculate which byte to load
			if (byte_offset != 0){
				if (byte_offset == 1){
					load_mask = 0x11110011;
				}
				else if (byte_offset == 2){
					load_mask = 0x11001111;
				}
				else{
					load_mask = 0x00111111;
				}
			}
			else {
				load_mask = 0x11111100;
			}	

			to_load = (*load_at & load_mask) >> byte_offset;

			if ((to_load >> 7) == 1){
			*/



/* LEGACY CODE - SB
			uint32_t byte_offset = reg[rs1]+imm % 4;
			uint32_t store_mask;	
			
			// calculate where in word to place byte
			if (byte_offset != 0){
				if (byte_offset == 1){
					store_mask = 0x11110011;
					to_store = to_store << 8;
				}
				else if (byte_offset == 2){
					store_mask = 0x11001111;
					to_store = to_store << 16;
				}
				else{
					store_mask = 0x00111111;
					to_store = to_store << 24;
				}
			}
			else {
				store_mask = 0x11111100;
			}

			// store byte
			*store_at = *store_at & store_mask; //Clear the section in which to store byte
			*store_at = *store_at & to_store;	//Store the byte, preserving the rest of the word */