#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

class SHA256{
	public:
		using uint = uint32_t;

		uint ROTR(uint x, int n) {
			return (x >> n) | (x << (32 - n));
		}

		uint SHR(uint x, int n) {
			return x >> n;
		}

		uint sigma0(uint x) {
			return ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3);
		}

		uint sigma1(uint x) {
			return ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10);
		}

		uint Sigma0(uint x) {
			return ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22);
		}

		uint Sigma1(uint x) {
			return ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25);
		}

		uint Ch(uint x, uint y, uint z) {
			return (x & y) ^ (~x & z);
		}

		uint Maj(uint x, uint y, uint z) {
			return (x & y) ^ (x & z) ^ (y & z);
		}

		int H[8] = {
			0x6a09e667,
			0xbb67ae85,
			0x3c6ef372,
			0xa54ff53a,
			0x510e527f,
			0x9b05688c,
			0x1f83d9ab,
			0x5be0cd19
		};

		int K[64] = {
			0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
			0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
			0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
			0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
			0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
			0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
			0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
			0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
		};

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
			int padding_size = 448 - (bits_size % 512); 	// if its for ex 335 we append 335 zeros
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

		std::vector<int> scheduleMessage(std::vector<int>& chunk) {
			std::vector<int> W(64);

			for (int i = 0; i < 16; i++) {
				int word = 0;
				for (int j = 0; j < 32; j++) {
					word = (word << 1) | chunk[i * 32 + j];
				}
				W[i] = word;
			}

			for (int i = 16; i < 64; i++) {
				W[i] = sigma1(W[i - 2]) + W[i - 7] + sigma0(W[i - 15]) + W[i - 16];
			}

			return W;
		}

		void processChunks(std::vector<std::vector<int>>& chunks) {
			for (const auto& chunk : chunks) {
				auto W = scheduleMessage(const_cast<std::vector<int>&>(chunk));
				int a = H[0], b = H[1], c = H[2], d = H[3];
				int e = H[4], f = H[5], g = H[6], h = H[7];

				for (int t = 0; t < 64; ++t) {
					int T1 = h + Sigma1(e) + Ch(e, f, g) + K[t] + W[t];
					int T2 = Sigma0(a) + Maj(a, b, c);
					h = g;
					g = f;
					f = e;
					e = d + T1;
					d = c;
					c = b;
					b = a;
					a = T1 + T2;
				}

				H[0] += a; H[1] += b; H[2] += c; H[3] += d;
				H[4] += e; H[5] += f; H[6] += g; H[7] += h;
			}
		}

		void printHash() {
        	for (int i = 0; i < 8; ++i) {
	            std::cout << std::hex << std::setw(8) << std::setfill('0') << H[i];
    	    }
        	std::cout << std::endl;
    	}
};
