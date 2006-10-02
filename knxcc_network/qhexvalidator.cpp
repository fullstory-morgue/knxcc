/*************************************************************************
 * Ralink Tech Inc.                                                      *
 * 4F, No. 2 Technology 5th Rd.                                          *
 * Science-based Industrial Park                                         *
 * Hsin-chu, Taiwan, R.O.C.                                              *
 *                                                                       *
 * (c) Copyright 2002, Ralink Technology, Inc.                           *
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  * 
 * it under the terms of the GNU General Public License as published by  * 
 * the Free Software Foundation; either version 2 of the License, or     * 
 * (at your option) any later version.                                   * 
 *                                                                       * 
 * This program is distributed in the hope that it will be useful,       * 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         * 
 * GNU General Public License for more details.                          * 
 *                                                                       * 
 * You should have received a copy of the GNU General Public License     * 
 * along with this program; if not, write to the                         * 
 * Free Software Foundation, Inc.,                                       * 
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             * 
 *                                                                       * 
 *************************************************************************

	Module Name:
	qhexvalidator.cpp

	Abstract:
		Implement hex validator for WEP security.

	Revision History:
	Who			When		  What
	--------	----------	  ----------------------------------------------
	Paul Wu		01-22-2003	  created
	Andreas Loibl   08-12-2006        adjusted for inputMask (NNNN-NNNN-NN;#)

*/


#include <qvalidator.h>
#include <qwidget.h>
#include <malloc.h>

#include "qhexvalidator.h"

QHexValidator::QHexValidator ( QWidget * parent, const char * name )
  : QValidator(parent, name)
{

}

QHexValidator::~QHexValidator()
{}

QValidator::State QHexValidator::validate ( QString &str, int &pos ) const
{
	char *tmp;
	char ch;
	unsigned int i;
	bool ok=FALSE;

	if(str.length() == 0)
		return QValidator::Acceptable;

	tmp = (char *)malloc(str.length()+1);	
	if(!tmp)
		return QValidator::Invalid;
	strcpy(tmp, str.data());
	for(i=0; i<str.length(); i++)
	{
		ch = *(tmp+i);		
		if( ((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <= 'F')) ||
		  ((ch >= '0') && (ch <= '9')) || (ch == '#') || (ch == '-') )
		{
			ok=TRUE;
		}
		else
		{
			ok=FALSE;
			break;
		}
	}

	//free(tmp);
	if(ok)
	{
		return QValidator::Acceptable;
	}
	else
	{
		return QValidator::Invalid;
	}
	pos=0; //avoid compile warning

}


