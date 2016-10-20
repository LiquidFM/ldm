/**
 * This file is part of ldm.
 *
 * Copyright (C) 2011-2016 Dmitriy Vilkov, <dav.daemon@gmail.com>
 *
 * ldm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ldm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ldm. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LDM_VISITOR_H_
#define LDM_VISITOR_H_

#include <platform/utils.h>


namespace LDM {

class Drive;
class HardDrive;
class FlashDrive;
class RemovableDrive;
class OpticalDrive;
class FloppyDrive;
class Partition;

class PLATFORM_MAKE_PUBLIC Visitor
{
public:
    Visitor();
    virtual ~Visitor();

    virtual void visit(Drive &item) = 0;
    virtual void visit(HardDrive &item) = 0;
    virtual void visit(FlashDrive &item) = 0;
    virtual void visit(RemovableDrive &item) = 0;
    virtual void visit(OpticalDrive &item) = 0;
    virtual void visit(FloppyDrive &item) = 0;
    virtual void visit(Partition &item) = 0;
};

}

#endif /* LDM_VISITOR_H_ */
