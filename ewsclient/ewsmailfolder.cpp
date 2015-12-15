/*  This file is part of Akonadi EWS Resource
    Copyright (C) 2015 Krzysztof Nowicki <krissn@op.pl>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "ewsfolderbase_p.h"
#include "ewsmailfolder.h"
#include "ewsclient_debug.h"

class EwsMailFolderPrivate : public EwsFolderBasePrivate
{
public:
    EwsMailFolderPrivate();
    EwsMailFolderPrivate(const EwsFolderBasePrivate &other);

    unsigned mUnreadCount;
};

EwsMailFolderPrivate::EwsMailFolderPrivate()
    : EwsFolderBasePrivate(), mUnreadCount(0)
{
    mType = EwsFolderTypeMail;
}

EwsMailFolder::EwsMailFolder(EwsFolderId id, EwsClient *parent)
    : EwsFolderBase(QSharedDataPointer<EwsFolderBasePrivate>(new EwsMailFolderPrivate()), id, parent)
{
}

EwsMailFolder::EwsMailFolder(QXmlStreamReader &reader, EwsClient *parent)
    : EwsFolderBase(QSharedDataPointer<EwsFolderBasePrivate>(new EwsMailFolderPrivate()), parent)
{
    EwsMailFolderPrivate *d = reinterpret_cast<EwsMailFolderPrivate*>(this->d.data());

    while (reader.readNextStartElement()) {
        if (reader.namespaceUri() != ewsTypeNsUri) {
            qCWarning(EWSCLIENT_LOG) << "Unexpected namespace in Folder element:"
                            << reader.namespaceUri();
            return;
        }

        if (reader.name() == QStringLiteral("UnreadCount")) {
            bool ok;
            d->mUnreadCount = reader.readElementText().toUInt(&ok);
            if (reader.error() != QXmlStreamReader::NoError || !ok) {
                qCWarning(EWSCLIENT_LOG) << QStringLiteral("Failed to read EWS request - invalid %1 element.")
                                .arg(QStringLiteral("UnreadCount"));
                return;
            }
        }
        else if (reader.name() == QStringLiteral("PermissionSet")) {
            // Unsupported - ignore
        }
        else if (!readBaseFolderElement(reader)) {
            qCWarning(EWSCLIENT_LOG).noquote() << QStringLiteral("Invalid Folder child: %1").arg(reader.qualifiedName().toString());
            return;
        }
    }
    d->mValid = true;
}

EwsMailFolder::EwsMailFolder(QSharedDataPointer<EwsFolderBasePrivate> priv, EwsClient *parent)
    : EwsFolderBase(priv, parent)
{
}

EwsMailFolder::~EwsMailFolder()
{
}

