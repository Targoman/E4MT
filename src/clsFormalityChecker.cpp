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

#include <QDir>
#include <sstream>
#include <vector>
#include <QMutexLocker>
#include "clsFormalityChecker.h"
#include "Configs.h"
#include "ISO639.h"
#include "libTargomanCommon/Logger.h"

namespace Targoman {
namespace Apps {

clsFormalityChecker::clsFormalityChecker()
{
    quint8 LangIndex = gConfigs::FastTextModelPattern.value().indexOf("%LANG%");
    foreach(QString File,
            QDir(gConfigs::FastTextModelPath.value()).entryList(QDir::Files | QDir::NoDotAndDotDot)){
        QString LangCode = File.mid(LangIndex, 2);
        if(ISO639isValid(LangCode.toLatin1().constData())){
            TargomanLogInfo(5, "Loading FastText models for: "<<ISO639getName(LangCode.toLatin1 ().constData ()));
            stuFastTextHolder& FTI = this->FastTextHolders[LangCode];
            if(!FTI.Lock){
                FTI.Lock = new QMutex;
                FTI.FastText = new fasttext::FastText;
                FTI.FastText->loadModel(
                        (gConfigs::FastTextModelPath.value() + "/"+ File).toStdString()
                        );
            }
        }else
            TargomanLogWarn(1, "Discarding invalid FastText model file: "<<File);
    }
}

QString clsFormalityChecker::check(const QString _lang, QString _text)
{
    stuFastTextHolder& FTI = this->FastTextHolders[_lang];
    if(FTI.Lock == NULL){
        TargomanLogError("unable to find appropiate instance for language: "<<_lang);
        return "formal";
    }

    std::stringstream SS;
    SS<<_text.replace("\n"," ").toUtf8().constData()<<"\n";
    SS.flush();

    std::vector<std::pair<fasttext::real,std::string>> Predictions;
    QMutexLocker Locker(FTI.Lock);

    FTI.FastText->predict(SS, 1, Predictions, gConfigs::FastTextThreshold.value());

    if(Predictions.size())
        return QString(Predictions[0].second.c_str()).replace("__label__","");
    else
        return "formal";
}

}
}
