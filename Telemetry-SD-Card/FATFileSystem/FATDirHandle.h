/* mbed Microcontroller Library
 * Copyright (c) 2006-2012 ARM Limited
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef MBED_FATDIRHANDLE_H
#define MBED_FATDIRHANDLE_H

#include <mbed_retarget.h>
#include "DirHandle.h"

using namespace mbed;

class FATDirHandle : public DirHandle {

 public:
    FATDirHandle(const FATFS_DIR &the_dir);
    virtual int closedir();
    virtual struct dirent *readdir();
    virtual void rewinddir();
    virtual off_t telldir();
    virtual void seekdir(off_t location);

    virtual ssize_t read(struct dirent *ent);    
    virtual int close() { return closedir(); };
    virtual void seek(off_t offset) { seekdir(offset); };
    virtual off_t tell() { return telldir(); };
    virtual void rewind() { rewinddir(); };

 private:
    FATFS_DIR dir;
    struct dirent cur_entry;

};

#endif
