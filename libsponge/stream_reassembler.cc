#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : buffer(capacity, '\0'), bitmap(capacity, false), _eof(false),
    unassemble_base(0), unassemble_bytes(0), _output(capacity), _capacity(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.

void StreamReassembler::push_byteStream() {
    string str = "";
    while (bitmap.front()) {
        // cout<<"check one more contiguous substring"<<endl;
        str += buffer.front();
        buffer.pop_front();
        bitmap.pop_front();
        buffer.push_back('\0');
        bitmap.push_back(false);
    }
    if (str.length() > 0) {
        // cout << "push one contiguous substring with length " << tmp.length() << endl;
        _output.write(str);
        unassemble_base += str.length();
        unassemble_bytes -= str.length();
    }
    return;
}
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if(eof) _eof = true;
    size_t len = data.length();
    // if (len == 0 && _eof && unassemble_bytes == 0) {
    //     _output.end_input();
    //     return;
    // }
    
    // invalid index
    if(index >= _capacity + unassemble_base) return;

    if(index >= unassemble_base) {
        size_t offset = index - unassemble_base;
        size_t copy_len = min(len, _capacity - _output.buffer_size() - offset);

        if(copy_len < len) {
            _eof = false;
        }
        for(size_t i = 0; i < copy_len; i++) {
            if(bitmap[i+offset]) continue;
            bitmap[i+offset] = true;
            buffer[i+offset] = data[i];
            unassemble_bytes++;
        }
    } else if(index + len  > unassemble_base) {
        size_t offset = unassemble_base - index;
        size_t copy_len = min(len - offset, _capacity - _output.buffer_size());
        if(copy_len < len - offset) {
            _eof = false;
        }
        for(size_t i = 0; i < copy_len; i++) {
            if(bitmap[i]) continue;
            bitmap[i] = true;
            buffer[i] = data[i+offset];
            unassemble_bytes++;
        }
    }
    push_byteStream();
    if (_eof && unassemble_bytes == 0) {
        _output.end_input();
    } 
    return;
}

size_t StreamReassembler::unassembled_bytes() const { return unassemble_bytes;}

bool StreamReassembler::empty() const { return unassemble_bytes == 0; }
