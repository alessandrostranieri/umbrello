 /*
  *  copyright (C) 2003-2004
  *  Umbrello UML Modeller Authors <uml-devel@ uml.sf.net>
  */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// own header
#include "enum.h"
// qt/kde includes
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
// app includes
#include "enumliteral.h"
#include "umldoc.h"
#include "uml.h"
#include "clipboard/idchangelog.h"

UMLEnum::UMLEnum(const QString& name, Uml::IDType id) : UMLClassifier(name, id) {
	init();
}

UMLEnum::~UMLEnum() {
	m_List.clear();
}

bool UMLEnum::operator==( UMLEnum & rhs ) {
	return UMLClassifier::operator==(rhs);
}

void UMLEnum::copyInto(UMLEnum *rhs) const
{
	UMLClassifier::copyInto(rhs);
}

UMLObject* UMLEnum::clone() const
{
	UMLEnum *clone = new UMLEnum();
	copyInto(clone);

	return clone;
}

void UMLEnum::init() {
	m_BaseType = Uml::ot_Enum;
	setStereotype( i18n("enum") );
}

UMLObject* UMLEnum::createEnumLiteral() {
	QString currentName = uniqChildName(Uml::ot_EnumLiteral);
	UMLEnumLiteral* newEnumLiteral = new UMLEnumLiteral(this, currentName);

	bool ok = true;
	bool goodName = false;

	while (ok && !goodName) {
		ok = newEnumLiteral->showPropertiesDialogue( UMLApp::app() );
		QString name = newEnumLiteral->getName();

		if(name.length() == 0) {
			KMessageBox::error(0, i18n("That is an invalid name."), i18n("Invalid Name"));
		} else {
			goodName = true;
		}
	}

	if (!ok) {
		return NULL;
	}

	addEnumLiteral(newEnumLiteral);

	UMLDoc *umldoc = UMLApp::app()->getDocument();
	umldoc->signalUMLObjectCreated(newEnumLiteral);
	return newEnumLiteral;
}

UMLObject* UMLEnum::addEnumLiteral(const QString &name, Uml::IDType id) {
	UMLEnumLiteral* literal = new UMLEnumLiteral(this, name, id);
	m_List.append(literal);
	emit modified();
	connect(literal,SIGNAL(modified()),this,SIGNAL(modified()));
	emit enumLiteralAdded(literal);
	return literal;
}

bool UMLEnum::addEnumLiteral(UMLEnumLiteral* literal, IDChangeLog* Log /* = 0*/) {
	QString name = (QString)literal->getName();
	if (findChildObject(Uml::ot_EnumLiteral, name).count() == 0) {
		literal->parent()->removeChild(literal);
		this->insertChild(literal);
		m_List.append(literal);
		emit modified();
		connect(literal,SIGNAL(modified()),this,SIGNAL(modified()));
		emit enumLiteralAdded(literal);
		return true;
	} else if (Log) {
		Log->removeChangeByNewID( literal->getID() );
		delete literal;
	}
	return false;
}

bool UMLEnum::addEnumLiteral(UMLEnumLiteral* literal, int position) {
	QString name = (QString)literal->getName();
	if (findChildObject( Uml::ot_EnumLiteral, name).count() == 0) {
		literal->parent()->removeChild(literal);
		this->insertChild(literal);
		if ( position >= 0 && position <= (int)m_List.count() )  {
			m_List.insert(position,literal);
		} else {
			m_List.append(literal);
		}
		emit modified();
		connect(literal,SIGNAL(modified()),this,SIGNAL(modified()));
		emit enumLiteralAdded(literal);
		return true;
	}
	return false;
}

int UMLEnum::removeEnumLiteral(UMLObject* literal) {
	if (!m_List.remove((UMLEnumLiteral*)literal)) {
		kdDebug() << "can't find att given in list" << endl;
		return -1;
	}
	emit enumLiteralRemoved(literal);
	emit modified();
	// If we are deleteing the object, then we dont need to disconnect..this is done auto-magically
	// for us by QObject. -b.t.
	// disconnect(a,SIGNAL(modified()),this,SIGNAL(modified()));
	delete literal;
	return m_List.count();
}

UMLEnumLiteral* UMLEnum::takeEnumLiteral(UMLEnumLiteral* el, int *wasAtIndex) {
	int index = m_List.findRef( el );
	if (wasAtIndex)
		*wasAtIndex = index;
	el = (index == -1 ? 0 : dynamic_cast<UMLEnumLiteral*>(m_List.take( )));
	if (el) {
		emit enumLiteralRemoved(el);
		emit modified();
	}
	return el;
}

UMLObjectList UMLEnum::findChildObject(Uml::Object_Type t, const QString &n) {
	UMLObjectList list;
	if (t == Uml::ot_Association) {
		return UMLClassifier::findChildObject(t, n);
	} else if (t == Uml::ot_EnumLiteral) {
		UMLClassifierListItem * obj=0;
		for (obj = m_List.first(); obj != 0; obj = m_List.next()) {
			if (obj->getBaseType() != t)
				continue;
			if (obj->getName() == n)
				list.append( obj );
		}
	} else {
		kdWarning() << "finding child object of unknown type" <<t<<" (requested name = "<<n<<")"<<endl;
	}

	return list;
}

int UMLEnum::enumLiterals() {
	return m_List.count();
}


void UMLEnum::saveToXMI(QDomDocument& qDoc, QDomElement& qElement) {
	QDomElement enumElement = UMLObject::save("UML:Enumeration", qDoc);
	//save operations
	UMLClassifierListItem* pEnumLiteral = 0;
	for ( pEnumLiteral = m_List.first(); pEnumLiteral != 0;
	      pEnumLiteral = m_List.next() ) {
		pEnumLiteral->saveToXMI(qDoc, enumElement);
	}
	qElement.appendChild(enumElement);
}

bool UMLEnum::load(QDomElement& element) {
	QDomNode node = element.firstChild();
	while( !node.isNull() ) {
		if (node.isComment()) {
			node = node.nextSibling();
			continue;
		}
		QDomElement tempElement = node.toElement();
		QString tag = tempElement.tagName();
		if (Uml::tagEq(tag, "EnumerationLiteral") ||
		    Uml::tagEq(tag, "EnumLiteral")) {   // for backward compatibility
			UMLEnumLiteral* pEnumLiteral = new UMLEnumLiteral(this);
			if( !pEnumLiteral->loadFromXMI(tempElement) ) {
				return false;
			}
			m_List.append(pEnumLiteral);
		} else if (tag == "stereotype") {
			kdDebug() << "UMLEnum::load(" << m_Name
				  << "): losing old-format stereotype." << endl;
		} else {
			kdWarning() << "unknown child type in UMLEnum::load" << endl;
		}
		node = node.nextSibling();
	}//end while
	return true;
}


#include "enum.moc"
