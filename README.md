# E4MT
An Essential text processor for Machine translation task

### Table of contents
- [Motivation](#motivation)
- [Sample codes](#sample-codes)
- [Features](#features)
- [Setup](#setup)
- [License](#license)


# Motivation
Text preparation is an essential step in training NLP models. This preparation can vary depending on the task and, include one or more of the following: Parsing, POS tagging, lemmatizing, tokenizing, etc.
Dealing with Persian language, a series of basic text preparations are required. Normalizing characters is one of them. Since, the Persian alphabet is based on the Arabic alphabet as do many other middle eastern languages, many non-Persian characters have been entered into Persian written text. This causes the same Persian entity to have different written forms which enlarges the vocabulary size of the Persian language.

Another preparation is related to the polymorphic nature of the Persian language. Persian is an agglutinative language and contains a large number of compound words that consist of a base and one or more affixes. 
Different authors have different manners of dealing with these words. Some use "zwnj"(zero-width non-joiner<0x200c>) character as delimiter between base and affix, some use space character as delimiter and some concatenate the affixes to the base. Again, this cause the same Persian entity to have different written forms. We handle such issues in E4MT. More details can be found in our [paper]().

********************************



# Sample codes

After building E4MT, according to [setup](#setup), it can be used on Linux by executing the following commands:
```bash
export E4MTBinPath=E4MT/out/bin
export LD_LIBRARY_PATH=$E4MTBinPath/../lib
$E4MTBinPath/E4MT -f /path/to/input/file -o /path/to/output/file -l fa -c scripts/E4MT.conf
```
Possible [options] can be found by:
```bash
$E4MTBinPath/E4MT -h
```

# Features
* Normalization

  E4MT normalizer converts different forms of characters to their canonical form. This operation covers the all UTF-8 2-byte characters. For example, the Persian   letter “ ‫ك‬ ” has 32 different Unicodes!)
  
* Spell correction 

  A module for correcting common Persian spelling mistakes. Modifications include:
    * Identify compound words and separate their parts using "zwnj" character. (Using predefined lists containing verb stems, adjectives, adverbs, and nouns, the spell correction module examines n-grams present in the sentence to determine   if it can be decomposed using any valid inflection rule. The identified segments are then concatenated using "zwnj" character.)
    * Correction of common spelling mistakes in Persian language like "سپاسگذار" instead of "سپاسگزار".
    * Correction of some common spelling mistakes frequently encountered in Persian digital contents. For example, a common mistake in building PDF from Persian documents is that glyph of characters are saved instead of their Unicodes.
  
* Entity tagging

  Limited set of entities (such as numbers, URLs, emails, list bullets and numberings, abbreviations, ...) is tagged to identify the punctuations and end of sentences. This module can be used to prepare texts for tasks that require text at the sentence level. It also can be used for tokenization.

* Tokenization/detokenization

  Using spelling correction and entity tagging to tokenize/detokenize, and at the same time convert Latin/Arabic numbers and punctuations to their equivalent.


# Setup

Follow the steps below to compile the project:
1.  Clone a fresh copy from github:
    ```bash
    git clone --recursive https://github.com/Targoman/E4MT.git
    ```
2. Install Qt5, libxml and libz. 
    Assuming a fresh Ubuntu:20.04 installation, following command does the job:
    ```bash
    apt install -y build-essential git libqt5core5a libqt5network5 cmake qt5-qmake qtbase5-dev qt5-default libxml2-dev zlib1g-dev
    ```
    or if you've installed Opensuse/leap:15, following command can be used:
    ```bash
    zypper install -y which libxml2-devel zlib-devel libQt5Core-devel libQt5Network-devel
    ```
3.  The project is a standard QMake out-of-source build, which on Linux can be compiled by executing the following commands:
    ```bash
    qmake
    make -j
    ```
# License
E4MT is published under the terms of [LGPLv3 License](./LICENSE)

