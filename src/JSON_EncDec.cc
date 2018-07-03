/******************************************************************************
* Copyright (c) 2015, 2015  Ericsson AB
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v2.0
* which accompanies this distribution, and is available at
* https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
*
* Contributors:
*  Gabor Szalai  - initial implementation and initial documentation
******************************************************************************/
//  File:          JSON_EncDec.cc
//  References:    RFC 4627 July 2006, json.org
//  Rev:           R1A
//  Prodnr:        CNL 113 676/1

#include <strings.h>
#include <stdio.h>

#include "JSON_Types.hh"

namespace JSON__Types {

  // function to handle escape characters in a JSON string
  void f__JSON__handleEscapes__string(JSON__String& input, const JSON__EscapeAction& action)
  {
    if (TTCN_Logger::log_this_event(TTCN_Logger::DEBUG_ENCDEC)) {
        TTCN_Logger::begin_event(TTCN_Logger::DEBUG_ENCDEC);
        TTCN_Logger::log_event_str("f__JSON__handleEscapes__string(): String before: ");
        input.log();
        TTCN_Logger::end_event();
    }

    TTCN_Buffer in_buffer;
    input.encode_utf8(in_buffer);
    in_buffer.put_c(0);

    unsigned int orig_len = in_buffer.get_len();
    const char* pos = (const char*) in_buffer.get_data();

    TTCN_Buffer ttcn_buffer;

    if(action == JSON__EscapeAction::escape) {

        char buf[7]; // length: 5 == \ + u + hex_code + \0
        while(*pos) { // go until \0; JSON_String == CHARSTRING ends with \0
            if(*pos < 31) { // control characters
                switch(*pos) {
                  case '\b':
                    ttcn_buffer.put_s(2, (unsigned char*)"\\b");
                    break;
                  case '\f':
                    ttcn_buffer.put_s(2, (unsigned char*)"\\f");
                    break;
                  case '\n':
                    ttcn_buffer.put_s(2, (unsigned char*)"\\n");
                    break;
                  case '\r':
                    ttcn_buffer.put_s(2, (unsigned char*)"\\r");
                    break;
                  case '\t':
                    ttcn_buffer.put_s(2, (unsigned char*)"\\t");
                    break;
                  default:
                    sprintf(buf, "\\u00%02x", *pos);
                    ttcn_buffer.put_s(6, (unsigned char*)buf);
                    break;
                }

            } else {

                switch(*pos) {

                case '"': // quotation mark (")
                  ttcn_buffer.put_s(2, (unsigned char*)"\\\"");
                  break;

                case '\\': // reverse solidus (\)
                  ttcn_buffer.put_s(2, (unsigned char*)"\\\\");
                  break;
                case '/': // solidus (/), The solidus, is on the list of the escaped charaters, but allowed in the string. So not escaped, only recognized for de-escaping.
                default:
                  ttcn_buffer.put_c(*pos);
                  break;

                }

            }

            pos++;

        }

    } else if(action == JSON__EscapeAction::unescape) {

        unsigned int ascii_code;

        while(*pos) { // go until \0 - 1 (\r == 2 char); JSON_String == CHARSTRING ends with \0

            if(*pos == '\\') { // escaped character starts with 0x5C == backslash
            
                switch(*(pos+1)) {
                  case 'b':
                    ttcn_buffer.put_c('\b');
                    pos++; // just leave out the backslash
                    break;
                  case 'f':
                    ttcn_buffer.put_c('\f');
                    pos++; // just leave out the backslash
                    break;
                  case 'n':
                    ttcn_buffer.put_c('\n');
                    pos++; // just leave out the backslash
                    break;
                  case 'r':
                    ttcn_buffer.put_c('\r');
                    pos++; // just leave out the backslash
                    break;
                  case 't':
                    ttcn_buffer.put_c('\t');
                    pos++; // just leave out the backslash
                    break;
                  case 'u':
                    if(strlen(pos) > 5) { // check if we have at least 5 bytes in the string

                        sscanf(pos + 1, "u00%02x", &ascii_code);
                        ttcn_buffer.put_c(ascii_code); // use the last two bytes; beware: CHARSTRING does not allow ASCII codes bigger than 127
                        pos += 5; // leave out the escaped sequence -1, since we increase pos by one at the end of the while loop

                    }
                    break;
                  case '\0':
                    // do nothing, this should never be happened
                    break;
                  default:  // the '\' and the '/' processed here
                    ttcn_buffer.put_c(*(pos + 1));
                    pos++; // just leave out the backslash
                    break;
                }

            } else {

                ttcn_buffer.put_c(*pos);

            }

            pos++;

        }

    }

    if(ttcn_buffer.get_len() != orig_len) { // if there were escaped characters

        input.decode_utf8(ttcn_buffer.get_len(),ttcn_buffer.get_data());

    }

    if (TTCN_Logger::log_this_event(TTCN_Logger::DEBUG_ENCDEC)) {
        TTCN_Logger::begin_event(TTCN_Logger::DEBUG_ENCDEC);
        TTCN_Logger::log_event_str("f__JSON__handleEscapes__string(): String after: ");
        input.log();
        TTCN_Logger::end_event();
    }

  }

}
