#include <iostream>
#include <vector>
#include <string>

class SHA256{
	public:
		std::vector<int> getBinary(std::string& message){
			std::vector<int> bits;

			for(char c : message){
				for(int i=7; i>=0; --i){
					bits.push_back((c >> i) & 1);			// stored like: ['1','0','1','1','0','0','0']
				}
			}
			return bits;
		}

		std::vector<int> padding(std::vector<int>& bits){
			bits.push_back(1); 								// stored like: ['1','0','1','1','0','0','0'] + 1 // Add 1 for padding
		 	int bits_size = bits.size();
			// 512 - 64 (holds original) = 448
			int padding_size = 448 - (bits_size % 512); 		// if its for ex 335 we append 335 zeros
			for(int i=0; i<padding_size; i++){
				bits.push_back(0);
			}
			int message_length_bits = bits_size;  			// Store the original message length in bits
        		for (int i = 63; i >= 0; --i) {
            			bits.push_back((message_length_bits >> i) & 1);
        		}

        		return bits;
	}

		std::vector<std::vector<int>> getChunks(std::vector<int>& bits){
			std::vector<std::vector<int>> chunks;

			for(int i=0; i<bits.size(); i+=512){
				std::vector<int> chunk;
				for(int j=0; j<512; j++){
					chunk.push_back(bits[i+j]);
				}
				chunks.push_back(chunk);
			}
			return chunks;
		}
};
