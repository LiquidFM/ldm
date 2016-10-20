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

#include "ldm_FloppyDrive.h"
#include "ldm_Visitor.h"


namespace LDM {

FloppyDrive::FloppyDrive(const Id &id,
  	    				 const QIcon &icon,
  	    				 const QString &label,
  	    				 bool hidden,
  	    				 Device *parent,
  	    				 quint64 size,
  	    				 MediaTypeSet mediaCompatibility,
  	    				 MediaType media,
  	    				 bool detachable) :
	RemovableDrive(id, icon, label, hidden, parent, size, mediaCompatibility, media, detachable)
{}

const FloppyDrive::MediaTypeSet &FloppyDrive::mediaTypeSet()
{
	static MediaTypeSet set = MediaTypeSet::fromList(
			MediaTypeList() << Floppy
			                << Floppy_Zip
			                << Floppy_Jaz);
	return set;
}

void FloppyDrive::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

}
