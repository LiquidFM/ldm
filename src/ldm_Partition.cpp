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

#include "ldm_Partition.h"
#include "ldm_Drive.h"
#include "ldm_Visitor.h"


namespace LDM {

Partition::Partition(const Id &id,
		  	  	  	 const QIcon &icon,
		  	  	  	 const QString &label,
		  	  	  	 bool hidden,
		  	  	  	 Drive *parent,
		  	  	  	 int number,
		  	  	  	 quint64 size,
		  	  	  	 const QStringList &mountPaths) :
	Device(id, icon, label, hidden, parent),
	m_mountPaths(mountPaths),
	m_number(number),
	m_size(size)
{}

void Partition::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

bool Partition::mount(QString &error)
{
	error = tr("Not implemented");
	return false;
}

bool Partition::unmount(QString &error)
{
	error = tr("Not implemented");
	return false;
}

}
