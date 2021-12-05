// render - A C++ OpenGL library
// Copyright (C) 2021  Intrets
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <cstdint>
#include <type_traits>
#include <cmath>

constexpr uint8_t toLinear(uint8_t x) {
	if (!std::is_constant_evaluated()) {
		const float v = x / 255.0f;

		if (v <= 0.04045f) {
			return static_cast<uint8_t>((v / 12.92f) * 255.0f + 0.5f);
		}
		else {
			return static_cast<uint8_t>(std::pow((v + 0.055f) / 1.055f, 2.4f) * 255.0f + 0.5f);
		}
	}
	else {
		switch (x) {
			case 0: return 0;
			case 1: return 0;
			case 2: return 0;
			case 3: return 0;
			case 4: return 0;
			case 5: return 0;
			case 6: return 0;
			case 7: return 1;
			case 8: return 1;
			case 9: return 1;
			case 10: return 1;
			case 11: return 1;
			case 12: return 1;
			case 13: return 1;
			case 14: return 1;
			case 15: return 1;
			case 16: return 1;
			case 17: return 1;
			case 18: return 2;
			case 19: return 2;
			case 20: return 2;
			case 21: return 2;
			case 22: return 2;
			case 23: return 2;
			case 24: return 2;
			case 25: return 2;
			case 26: return 3;
			case 27: return 3;
			case 28: return 3;
			case 29: return 3;
			case 30: return 3;
			case 31: return 3;
			case 32: return 4;
			case 33: return 4;
			case 34: return 4;
			case 35: return 4;
			case 36: return 4;
			case 37: return 5;
			case 38: return 5;
			case 39: return 5;
			case 40: return 5;
			case 41: return 6;
			case 42: return 6;
			case 43: return 6;
			case 44: return 6;
			case 45: return 7;
			case 46: return 7;
			case 47: return 7;
			case 48: return 8;
			case 49: return 8;
			case 50: return 8;
			case 51: return 8;
			case 52: return 9;
			case 53: return 9;
			case 54: return 9;
			case 55: return 10;
			case 56: return 10;
			case 57: return 10;
			case 58: return 11;
			case 59: return 11;
			case 60: return 12;
			case 61: return 12;
			case 62: return 12;
			case 63: return 13;
			case 64: return 13;
			case 65: return 13;
			case 66: return 14;
			case 67: return 14;
			case 68: return 15;
			case 69: return 15;
			case 70: return 16;
			case 71: return 16;
			case 72: return 17;
			case 73: return 17;
			case 74: return 17;
			case 75: return 18;
			case 76: return 18;
			case 77: return 19;
			case 78: return 19;
			case 79: return 20;
			case 80: return 20;
			case 81: return 21;
			case 82: return 22;
			case 83: return 22;
			case 84: return 23;
			case 85: return 23;
			case 86: return 24;
			case 87: return 24;
			case 88: return 25;
			case 89: return 25;
			case 90: return 26;
			case 91: return 27;
			case 92: return 27;
			case 93: return 28;
			case 94: return 29;
			case 95: return 29;
			case 96: return 30;
			case 97: return 30;
			case 98: return 31;
			case 99: return 32;
			case 100: return 32;
			case 101: return 33;
			case 102: return 34;
			case 103: return 35;
			case 104: return 35;
			case 105: return 36;
			case 106: return 37;
			case 107: return 37;
			case 108: return 38;
			case 109: return 39;
			case 110: return 40;
			case 111: return 41;
			case 112: return 41;
			case 113: return 42;
			case 114: return 43;
			case 115: return 44;
			case 116: return 45;
			case 117: return 45;
			case 118: return 46;
			case 119: return 47;
			case 120: return 48;
			case 121: return 49;
			case 122: return 50;
			case 123: return 51;
			case 124: return 51;
			case 125: return 52;
			case 126: return 53;
			case 127: return 54;
			case 128: return 55;
			case 129: return 56;
			case 130: return 57;
			case 131: return 58;
			case 132: return 59;
			case 133: return 60;
			case 134: return 61;
			case 135: return 62;
			case 136: return 63;
			case 137: return 64;
			case 138: return 65;
			case 139: return 66;
			case 140: return 67;
			case 141: return 68;
			case 142: return 69;
			case 143: return 70;
			case 144: return 71;
			case 145: return 72;
			case 146: return 73;
			case 147: return 74;
			case 148: return 76;
			case 149: return 77;
			case 150: return 78;
			case 151: return 79;
			case 152: return 80;
			case 153: return 81;
			case 154: return 82;
			case 155: return 84;
			case 156: return 85;
			case 157: return 86;
			case 158: return 87;
			case 159: return 88;
			case 160: return 90;
			case 161: return 91;
			case 162: return 92;
			case 163: return 93;
			case 164: return 95;
			case 165: return 96;
			case 166: return 97;
			case 167: return 99;
			case 168: return 100;
			case 169: return 101;
			case 170: return 103;
			case 171: return 104;
			case 172: return 105;
			case 173: return 107;
			case 174: return 108;
			case 175: return 109;
			case 176: return 111;
			case 177: return 112;
			case 178: return 114;
			case 179: return 115;
			case 180: return 116;
			case 181: return 118;
			case 182: return 119;
			case 183: return 121;
			case 184: return 122;
			case 185: return 124;
			case 186: return 125;
			case 187: return 127;
			case 188: return 128;
			case 189: return 130;
			case 190: return 131;
			case 191: return 133;
			case 192: return 134;
			case 193: return 136;
			case 194: return 138;
			case 195: return 139;
			case 196: return 141;
			case 197: return 142;
			case 198: return 144;
			case 199: return 146;
			case 200: return 147;
			case 201: return 149;
			case 202: return 151;
			case 203: return 152;
			case 204: return 154;
			case 205: return 156;
			case 206: return 157;
			case 207: return 159;
			case 208: return 161;
			case 209: return 163;
			case 210: return 164;
			case 211: return 166;
			case 212: return 168;
			case 213: return 170;
			case 214: return 171;
			case 215: return 173;
			case 216: return 175;
			case 217: return 177;
			case 218: return 179;
			case 219: return 181;
			case 220: return 183;
			case 221: return 184;
			case 222: return 186;
			case 223: return 188;
			case 224: return 190;
			case 225: return 192;
			case 226: return 194;
			case 227: return 196;
			case 228: return 198;
			case 229: return 200;
			case 230: return 202;
			case 231: return 204;
			case 232: return 206;
			case 233: return 208;
			case 234: return 210;
			case 235: return 212;
			case 236: return 214;
			case 237: return 216;
			case 238: return 218;
			case 239: return 220;
			case 240: return 222;
			case 241: return 224;
			case 242: return 226;
			case 243: return 229;
			case 244: return 231;
			case 245: return 233;
			case 246: return 235;
			case 247: return 237;
			case 248: return 239;
			case 249: return 242;
			case 250: return 244;
			case 251: return 246;
			case 252: return 248;
			case 253: return 250;
			case 254: return 253;
			case 255: return 255;
		}
	}
}
