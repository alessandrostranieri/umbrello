
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*  This code generated by:
 *      Author : thomas
 *      Date   : Mon Sep 1 2003
 */


#ifndef CPPHEADERCODEOPERATION_H
#define CPPHEADERCODEOPERATION_H

#include <qstring.h>
#include "../codeoperation.h"

class CPPHeaderCodeDocument;

class CPPHeaderCodeOperation : virtual public CodeOperation
{
    Q_OBJECT
public:

    // Constructors/Destructors
    //

    /**
     * Constructor
     */
    CPPHeaderCodeOperation ( CPPHeaderCodeDocument * doc, UMLOperation * op,
                             const QString & body = "", const QString & comment = "");

    /**
     * Empty Destructor
     */
    virtual ~CPPHeaderCodeOperation ( );

    virtual int lastEditableLine();

protected:

    virtual void updateMethodDeclaration();
    virtual void updateContent( );

    /**
     * Check to see if we have a valid stereotype to apply in the operation
     */
    virtual void applyStereotypes (QString&, UMLOperation *, bool, bool, QString&, QString&);

private:
    void init (CPPHeaderCodeDocument * doc );


};

#endif // CPPHEADERCODEOPERATION_H
