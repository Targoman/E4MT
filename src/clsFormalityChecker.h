/******************************************************************************
 * Targoman: A robust Machine Translation framework               *
 *                                                                            *
 * Copyright 2014-2018 by ITRC <http://itrc.ac.ir>                            *
 *                                                                            *
 * This file is part of Targoman.                                             *
 *                                                                            *
 * Targoman is free software: you can redistribute it and/or modify           *
 * it under the terms of the GNU Lesser General Public License as published   *
 * by the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                        *
 *                                                                            *
 * Targoman is distributed in the hope that it will be useful,                *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU Lesser General Public License for more details.                        *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with Targoman. If not, see <http://www.gnu.org/licenses/>.           *
 *                                                                            *
 ******************************************************************************/
/**
 * @author S. Mohammad M. Ziabary <ziabary@targoman.com>
 */

#ifndef CLSFORMALITYCHECKER_H
#define CLSFORMALITYCHECKER_H

#include <QMap>
#include <QMutex>
#include <QScopedPointer>
#include "fastText/src/fasttext.h"

namespace Targoman {
namespace Apps {

class clsFormalityChecker
{
private:
    struct stuFastTextHolder{
        QMutex* Lock;
        fasttext::FastText* FastText;
        stuFastTextHolder(): Lock(NULL), FastText(NULL)
        {}
        ~stuFastTextHolder(){
            if(Lock)     delete Lock;
            if(FastText) delete FastText;
        }
    };

public:
    clsFormalityChecker();
    QString check(const QString _lang, QString _text);

private:
    QMap<QString, stuFastTextHolder> FastTextHolders;
};

}
}
#endif // CLSFORMALITYCHECKER_H
