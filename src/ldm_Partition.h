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

#ifndef LDM_PARTITION_H_
#define LDM_PARTITION_H_

#include <ldm/Device>
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>


namespace LDM {

class Drive;

class PLATFORM_MAKE_PUBLIC Partition : public Device
{
	Q_DECLARE_TR_FUNCTIONS(Partition)

public:
	Partition(const Id &id,
			  const QIcon &icon,
			  const QString &label,
			  bool hidden,
			  Drive *parent,
			  int number,
			  quint64 size,
			  const QStringList &mountPaths);

	int number() const { return m_number; }
	void setNumber(int number) { m_number = number; }

	quint64 size() const { return m_size; }
	void setSize(quint64 size) { m_size = size; }

	const QStringList &mountPaths() const { return m_mountPaths; }
	void setMountPaths(const QStringList &mountPaths) { m_mountPaths = mountPaths; }

    virtual void accept(Visitor &visitor);
	virtual bool mount(QString &error);
	virtual bool unmount(QString &error);

protected:
	QStringList m_mountPaths;

private:
	int m_number;
	quint64 m_size;
};

}

#endif /* LDM_PARTITION_H_ */
