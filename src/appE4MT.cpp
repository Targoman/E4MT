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

#include <iostream>
#include <sstream>

#include "appE4MT.h"
#include "libTargomanTextProcessor/TextProcessor.h"
#include "XMLReader.h"
#include "Configs.h"

namespace Targoman {
namespace Apps {

using namespace NLPLibs;
using namespace Common;
using namespace Common::Configuration;

thread_local static QList<stuIXMLReplacement> SentenceBreakReplacements;


void appE4MT::slotExecute()
{
    try{
        if (gConfigs::BreakLines.value())
            SentenceBreakReplacements.append(
                        stuIXMLReplacement(
                            QRegularExpression("(\\s)([\\.\\?\\!])(\\s)"),
                            "\\1\\2\n\\3"));

        if (gConfigs::Mode.value() == enuAppMode::Server){
            connect(&ConfigManager::instance(),
                    &ConfigManager::sigValidateAgent,
                    this,
                    &appE4MT::slotValidateAgent,
                    Qt::DirectConnection);
            connect(&ConfigManager::instance(),
                    &ConfigManager::sigPing,
                    this,
                    &appE4MT::slotPong,
                    Qt::DirectConnection);
            TargomanTextProcessor::instance().init(ConfigManager::instance().configSettings());
            FormalityChecker.reset(new clsFormalityChecker);
            ConfigManager::instance().startAdminServer();
            return;
        }else{
            bool SpellCorrected;
            if (gConfigs::Input.value().size()){
                TargomanTextProcessor::instance().init(ConfigManager::instance().configSettings());
                switch(gConfigs::Mode.value()){
                case enuAppMode::Text2IXML:
                    std::cout<<TargomanTextProcessor::instance().text2IXML(
                                   gConfigs::Input.value(),
                                   SpellCorrected,
                                   gConfigs::Language.value(),
                                   0,
                                   false,
                                   (gConfigs::NoSpellcorrector.value() ? false : true),
                                   QList<enuTextTags::Type>(),
                                   SentenceBreakReplacements
                                   ).toUtf8().constData()<<std::endl;
                    break;
                case enuAppMode::IXML2Text:
                    std::cout<<TargomanTextProcessor::instance().ixml2Text(
                                   gConfigs::Input.value()).toUtf8().constData()<<std::endl;
                    break;

                case enuAppMode::Preprocess:{
                    FormalityChecker.reset(new clsFormalityChecker);
                    QString Normalized = std::get<1>(this->text2Ixml_Helper(
                                   QVariantList(),
                                   (gConfigs::NoSpellcorrector.value() ? false : true),
                                   gConfigs::Language.value(),
                                   gConfigs::Input.value()
                                   ));
                    std::cout<<FormalityChecker->check(gConfigs::Language.value(), Normalized).toUtf8().constData()
                             << "\t"
                             << Normalized.toUtf8().constData()<<std::endl;
                    break;
                }case enuAppMode::Tokenize:
                    std::cout<<TargomanTextProcessor::instance().ixml2Text(
                                   TargomanTextProcessor::instance().text2IXML(
                                       gConfigs::Input.value(),
                                       SpellCorrected,
                                       gConfigs::Language.value(),
                                       0,
                                       gConfigs::Interactive.value(),
                                       (gConfigs::NoSpellcorrector.value() ? false : true),
                                       QList<enuTextTags::Type>(),
                                       SentenceBreakReplacements
                                       )).toUtf8().constData()<<std::endl;
                    break;
                case enuAppMode::Normalize:
                    std::cout<<TargomanTextProcessor::instance().normalizeText(
                                   gConfigs::Input.value(),
                                   SpellCorrected,
                                   gConfigs::Language.value()).toUtf8().constData()<<std::endl;
                    break;
                default:
                    throw exAppE4MT("Invalid action selected for simple input");
                }
            }else if (gConfigs::InputFile.value().size()){
                TargomanTextProcessor::instance().init(ConfigManager::instance().configSettings());
                QFileInfo InputFileInfo(gConfigs::InputFile.value());
                this->processFile(gConfigs::InputFile.value(),
                                  (gConfigs::OutputPath.value().isEmpty() ?
                                      InputFileInfo.path() + '/' + InputFileInfo.fileName():
                                      gConfigs::OutputPath.value())
                                  );
            }else if (gConfigs::InputDir.value().size()){
                TargomanTextProcessor::instance().init(ConfigManager::instance().configSettings());
                this->processDir("/./", gConfigs::InputDir.value());
            }else
                throw exAppE4MT("No job defined to be done");

        }
        QCoreApplication::exit(0);
    }catch(Common::exTargomanBase& e){
        TargomanLogError(e.what());
        QCoreApplication::exit(-1);
    }
}

Targoman::Common::Configuration::stuRPCOutput appE4MT::rpcNormalize(const QVariantMap &_args)
{
    QString Text     = _args.value("txt").toString();
    QString Language = _args.value("lang").toString();
    if (Text.isEmpty())
        throw exAppE4MT("Invalid empty text");

    bool WasSpellCorrected;
    Text = TargomanTextProcessor::instance().normalizeText(Text, WasSpellCorrected, false, Language);
    QVariantMap Args;
    Args.insert("spell",WasSpellCorrected);
    return stuRPCOutput(Text, Args);
}

Targoman::Common::Configuration::stuRPCOutput appE4MT::rpcPreprocessText(const QVariantMap &_args){
    QString Text;
    QString Lang = _args.value("lang").toString();
    bool WasSpellCorrected;

    std::tie(WasSpellCorrected, Text) = this->text2Ixml_Helper(
                _args.value("rem").toList(),
                _args.value("spell",false).toBool(),
                Lang,
                _args.value("txt").toString());


    QVariantMap Args;
    Args.insert("spell",WasSpellCorrected);
    Args.insert("formality", this->FormalityChecker->check(Lang, Text));
    return stuRPCOutput(Text, Args);
}

std::tuple<bool, QString> appE4MT::text2Ixml_Helper(const QVariantList &_removalItems,
                               bool _useSpellCorrector,
                               QString _language,
                               QString _text)
{
    if (_text.isEmpty())
        throw exAppE4MT("Invalid empty text");
    QList<enuTextTags::Type> RemovingTags;
    foreach(const QVariant& TagVariant, _removalItems){
        enuTextTags::Type Tag = enuTextTags::toEnum(TagVariant.toString());
        if (Tag == enuTextTags::Unknown)
            throw exAppE4MT("Invalid Tag: " + TagVariant.toString());
        RemovingTags.append(Tag);
    }

    bool WasSpellCorrected;
    _text = TargomanTextProcessor::instance().text2IXML(_text,
                                                       WasSpellCorrected,
                                                       _language,
                                                       0,
                                                       false,
                                                       _useSpellCorrector,
                                                       RemovingTags,
                                                       SentenceBreakReplacements);

    return std::make_tuple(WasSpellCorrected, _text);
}

Targoman::Common::Configuration::stuRPCOutput appE4MT::rpcText2IXML(const QVariantMap &_args)
{
    QString Text;
    bool WasSpellCorrected;

    std::tie(WasSpellCorrected, Text) = this->text2Ixml_Helper(
                _args.value("rem").toList(),
                _args.value("spell",false).toBool(),
                _args.value("lang").toString(),
                _args.value("txt").toString());

    QVariantMap Args;
    Args.insert("spell",WasSpellCorrected);
    return stuRPCOutput(Text, Args);
}

Targoman::Common::Configuration::stuRPCOutput appE4MT::rpcIXML2Text(const QVariantMap &_args)
{
    QString IXML     = _args.value("txt").toString();
    if (IXML.isEmpty())
        throw exAppE4MT("Invalid empty text");

    return stuRPCOutput(TargomanTextProcessor::instance().ixml2Text(IXML));
}

Targoman::Common::Configuration::stuRPCOutput appE4MT::rpcTokenize(const QVariantMap &_args)
{
    QString Text     = _args.value("txt").toString();
    QString Language = _args.value("lang").toString();
    bool    UseSpellCorrector = _args.value("spell",false).toBool();
    if (Text.isEmpty())
        throw exAppE4MT("Invalid empty text");
    QList<enuTextTags::Type> RemovingTags;
    foreach(const QVariant& TagVar, _args.value("rem").toList()){
        enuTextTags::Type Tag = enuTextTags::toEnum(TagVar.toString());
        if (Tag == enuTextTags::Unknown)
            throw exAppE4MT("Invalid Tag: " + TagVar.toString());
        RemovingTags.append(Tag);
    }

    bool WasSpellCorrected;
    Text = TargomanTextProcessor::instance().text2IXML(Text,
                                                       WasSpellCorrected,
                                                       Language,
                                                       0,
                                                       false,
                                                       UseSpellCorrector,
                                                       RemovingTags,
                                                       SentenceBreakReplacements);

    Text = TargomanTextProcessor::instance().ixml2Text(Text);
    QVariantMap Args;
    Args.insert("spell",WasSpellCorrected);
    return stuRPCOutput(Text, Args);
}

void appE4MT::processDir(const QString &_relativeDir, const QString& _basePath)
{
    QDir Dir(_basePath + _relativeDir);
    const QList<QFileInfo>& SelectedFiles = Dir.entryInfoList(
                (gConfigs::Recursive.value() ? QDir::AllEntries : QDir::Files) | QDir::NoDotAndDotDot,
                QDir::Name | QDir::DirsFirst);
    foreach (const QFileInfo& FileInfo, SelectedFiles){
        if (FileInfo.isDir()){
            if(gConfigs::OutputPath.value().size())
                QDir().mkpath(gConfigs::OutputPath.value() + '/' + _relativeDir + '/' + FileInfo.fileName());
            this->processDir(_relativeDir + "/" + FileInfo.fileName(), _basePath);
        }else{
            if (gConfigs::IncludePattern.value().pattern().size() &&
                    gConfigs::IncludePattern.value().exactMatch(FileInfo.fileName()) == false)
                continue;
            this->processFile(gConfigs::InputDir.value() + '/' + _relativeDir + '/' + FileInfo.fileName(),
                              (gConfigs::OutputPath.value().isEmpty() ?
                                  gConfigs::InputDir.value()  : gConfigs::OutputPath.value())
                              + '/' + _relativeDir + '/' + FileInfo.fileName());
        }
    }
}

#define OPEN_OUT_STREAM(_extension) \
    if (gConfigs::OutputPath.value().isEmpty()) OutFile.open(stdout, QIODevice::WriteOnly); \
    else{ \
          if (gConfigs::InputFile.value().size()) \
                OutFile.setFileName(_outFile); \
          else \
                OutFile.setFileName(_outFile + '.' + _extension); \
          if(OutFile.exists() && gConfigs::Overwrite.value() == false){ \
             TargomanWarn(1, "Ignoring save to existent file: "<<OutFile.fileName()); \
             return; } \
          OutFile.open(QIODevice::WriteOnly); \
    } OutStream.setCodec("UTF8");


void appE4MT::processFile(const QString& _inputFile, const QString &_outFile)
{
    TargomanDebug(4,"<<<<<<<<<<<<< " + _inputFile);
    QFile OutFile;
    QTextStream OutStream(&OutFile);

    bool SpellCorrected;
    switch(gConfigs::Mode.value()){
    case enuAppMode::Validate:
        XMLReader::isValid(_inputFile);
        break;
    case enuAppMode::Text2IXML:
        OPEN_OUT_STREAM("ixml");
        foreach (const QString& Line, this->retrieveFileItems(_inputFile))
            OutStream<<TargomanTextProcessor::instance().text2IXML(
                           Line,
                           SpellCorrected,
                           gConfigs::Language.value(),
                           0,
                           false,
                           (gConfigs::NoSpellcorrector.value() ? false : true),
                           QList<enuTextTags::Type>(),
                           SentenceBreakReplacements
                           )<<"\n";
        break;
    case enuAppMode::IXML2Text:
        OPEN_OUT_STREAM("txt");
        foreach (const QString& Line, this->retrieveFileItems(_inputFile))
            OutStream<<TargomanTextProcessor::instance().ixml2Text(
                           Line,
                           "",
                           true,
                           true,
                           false)<<"\n";
        break;
    case enuAppMode::Tokenize:
        OPEN_OUT_STREAM("tokenized");
        foreach (const QString& Line, this->retrieveFileItems(_inputFile))
            OutStream<<TargomanTextProcessor::instance().ixml2Text(
                           TargomanTextProcessor::instance().text2IXML(
                               Line,
                               SpellCorrected,
                               gConfigs::Language.value(),
                               0,
                               false,
                               (gConfigs::NoSpellcorrector.value() ? false : true),
                               QList<enuTextTags::Type>(),
                               SentenceBreakReplacements
                               ),
                           "",
                           false,
                           false,
                           false)<<"\n";
        break;
    case enuAppMode::Normalize:
        OPEN_OUT_STREAM("normalized");
        foreach (const QString& Line, this->retrieveFileItems(_inputFile))
            OutStream<<TargomanTextProcessor::instance().normalizeText(
                           Line,
                           SpellCorrected,
                           gConfigs::Language.value())<<"\n";
        break;
    default:
        throw exAppE4MT("Invalid action selected for simple input");
    }
    TargomanDebug(4,">>>>>>>>>>>>> " << OutFile.fileName() );
}

QStringList appE4MT::retrieveFileItems(const QString &_filePath)
{
    QStringList Lines;
    if (QFileInfo(_filePath).suffix() != "xml" || gConfigs::PlainText.value()){
        QFile File(_filePath);
        File.open(QIODevice::ReadOnly);
        if (File.isReadable() == false)
            throw exAppE4MT("Unable to open: <" + _filePath + "> for reading");
        QTextStream Stream(&File);
        Stream.setCodec("UTF8");
        Lines = Stream.readAll().split("\n");
    }else
        Lines = XMLReader::getContext(_filePath, gConfigs::KeepTitles.value());
    return Lines;
}

void appE4MT::slotPong(QString _ssid, Targoman::Common::stuPong &_pong)
{
    Q_UNUSED(_ssid);
    _pong.Status = enuStatus::Ok;
    _pong.Message = "Ok";
}

void appE4MT::slotValidateAgent(QString &_user, const QString &_pass, const QString &_ip, bool &_canView, bool &_canChange)
{
    Q_UNUSED(_user)
    Q_UNUSED(_pass)
    Q_UNUSED(_ip)
    _canView = true;
    _canChange = false;
}

}
}
