/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

// This is not a normal header, *don't* add include guards to it.
// This will cause the compiler to get wrong offsets and to corrupt
// the stack.

protected:

void initDrawingML();

#include "MsooXmlDrawingMLShared.h"

// All the readers
#if !defined MSOOXMLDRAWINGTABLESTYLEREADER_H
KoFilter::ConversionStatus read_relIds();
KoFilter::ConversionStatus read_chart();
#endif
KoFilter::ConversionStatus read_pic();
KoFilter::ConversionStatus read_nvPicPr();
enum cNvPrCaller {
    cNvPr_nvSpPr,
    cNvPr_nvPicPr,
    cNvPr_nvCxnSpPr
};
KoFilter::ConversionStatus read_cNvPr(cNvPrCaller caller);
KoFilter::ConversionStatus read_cNvPicPr();
KoFilter::ConversionStatus read_nvSpPr();
KoFilter::ConversionStatus read_style();
KoFilter::ConversionStatus read_fillRef();

KoGenStyle m_referredFont;
QString m_referredFontName;
KoFilter::ConversionStatus read_fontRef();

KoFilter::ConversionStatus read_lnRef();
KoFilter::ConversionStatus read_cNvSpPr();
KoFilter::ConversionStatus read_nvCxnSpPr();
KoFilter::ConversionStatus read_grpSp();

bool unsupportedPredefinedShape();

KoFilter::ConversionStatus read_grpSpPr();
void preReadSp();
void generateFrameSp();
KoFilter::ConversionStatus read_cxnSp();
KoFilter::ConversionStatus read_sp();
KoFilter::ConversionStatus read_spPr();
KoFilter::ConversionStatus read_xfrm();
bool m_xfrm_read;
KoFilter::ConversionStatus read_off();
KoFilter::ConversionStatus read_chOff();
KoFilter::ConversionStatus read_chExt();
KoFilter::ConversionStatus read_ext();
KoFilter::ConversionStatus read_blip();
KoFilter::ConversionStatus read_stretch();
KoFilter::ConversionStatus read_biLevel();
KoFilter::ConversionStatus read_grayscl();
KoFilter::ConversionStatus read_lum();
KoFilter::ConversionStatus read_duotone();

KoFilter::ConversionStatus read_tile();
KoFilter::ConversionStatus read_srcRect();

KoFilter::ConversionStatus read_fillRect();
KoFilter::ConversionStatus read_graphic();
KoFilter::ConversionStatus read_graphicData();
enum blipFillCaller {
    blipFill_spPr = 'a',
    blipFill_pic = 'p', //dml in pptx; for dml in docx use 'pic'
    blipFill_rPr = 'p', //dml
    blipFill_bgPr = 'a', // pptx
    blipFill_grpSpPr = 'a'
};
KoFilter::ConversionStatus read_blipFill(blipFillCaller caller);

bool m_insideTable;
qreal m_maxParaFontPt; // Largest font size used in the paragraph
qreal m_minParaFontPt; // minimum font size used in the paragraph
KoFilter::ConversionStatus read_DrawingML_p();
read_p_args m_read_DrawingML_p_args;

void handleRprAttributes(const QXmlStreamAttributes& attrs);
KoFilter::ConversionStatus read_DrawingML_rPr();
KoFilter::ConversionStatus read_DrawingML_br();
KoFilter::ConversionStatus read_endParaRPr();

KoFilter::ConversionStatus read_hlinkClick();

KoFilter::ConversionStatus read_DrawingML_pPr();

KoFilter::ConversionStatus read_DrawingML_r();
KoFilter::ConversionStatus read_DrawingML_highlight();
KoFilter::ConversionStatus read_DrawingML_txBody();
KoFilter::ConversionStatus read_lstStyle();
KoFilter::ConversionStatus read_latin();
int m_gradPosition;
KoFilter::ConversionStatus read_gradFill();
KoFilter::ConversionStatus read_gradFillRpr();
QString m_gradAngle;
KoFilter::ConversionStatus read_lin();
KoFilter::ConversionStatus read_gsLst();
KoFilter::ConversionStatus read_gs();
KoFilter::ConversionStatus read_prstGeom();
KoFilter::ConversionStatus read_gd();
bool m_contentAvLstExists; // whether avLst exists
QMap<QString, QString> m_avModifiers;
KoFilter::ConversionStatus read_avLst();
KoFilter::ConversionStatus read_noFill();

KoFilter::ConversionStatus read_effectLst();
KoFilter::ConversionStatus read_outerShdw();

QString m_customPath;
QString m_customEquations;
QString m_textareas;
KoFilter::ConversionStatus read_custGeom();

QString m_contentType; // read in prstGeom

KoFilter::ConversionStatus read_fld();

enum spacingType {
    spacingMarginTop, spacingLines, spacingMarginBottom
};
spacingType m_currentSpacingType; // determines how spcPct and spcPts should behave

MSOOXML::Utils::autoFitStatus m_normAutofit; // Whether text should be fitted to fit the shape

KoFilter::ConversionStatus read_lnSpc();
KoFilter::ConversionStatus read_spcPct();
KoFilter::ConversionStatus read_spcBef();
KoFilter::ConversionStatus read_spcAft();
KoFilter::ConversionStatus read_spcPts();

QString m_shapeTextPosition;
QString m_shapeTextTopOff;
QString m_shapeTextBottomOff;
QString m_shapeTextLeftOff;
QString m_shapeTextRightOff;

bool m_listStylePropertiesAltered;
QString m_prevListStyleName;

int m_prevListLevel; //! set by drawingML_ppr
int m_currentListLevel; //! set by drawingML_ppr

// Map of level keys and xml:id values of text:list elements to continue
// automatic numbering.
QMap<quint16, QString> m_lvlXmlIdMap;

// true - continue numbered list, false - restart numbering
QMap<quint16, bool> m_continueListNumbering;

KoFilter::ConversionStatus read_buClr();
KoFilter::ConversionStatus read_buClrTx();
KoFilter::ConversionStatus read_buSzPct();
KoFilter::ConversionStatus read_buSzPts();
KoFilter::ConversionStatus read_buChar();
KoFilter::ConversionStatus read_buBlip();
KoFilter::ConversionStatus read_buNone();
KoFilter::ConversionStatus read_buFont();
KoFilter::ConversionStatus read_buAutoNum();
KoFilter::ConversionStatus lvlHelper(const QString& level);
KoFilter::ConversionStatus read_lvl1pPr();
KoFilter::ConversionStatus read_lvl2pPr();
KoFilter::ConversionStatus read_lvl3pPr();
KoFilter::ConversionStatus read_lvl4pPr();
KoFilter::ConversionStatus read_lvl5pPr();
KoFilter::ConversionStatus read_lvl6pPr();
KoFilter::ConversionStatus read_lvl7pPr();
KoFilter::ConversionStatus read_lvl8pPr();
KoFilter::ConversionStatus read_lvl9pPr();
KoFilter::ConversionStatus read_defRPr();
KoFilter::ConversionStatus read_bodyPr();
KoFilter::ConversionStatus read_normAutofit();
KoFilter::ConversionStatus read_spAutoFit();

KoFilter::ConversionStatus read_masterClrMapping();
KoFilter::ConversionStatus read_overrideClrMapping();

//! Sets style:wrap attribute of style:style/style:graphic-properties element. Used in read_anchor()
void saveStyleWrap(const char * style);

void algnToODF(const char * odfEl, const QString& emuValue);

//! Sets fo:margin-* attribute of style:style/style:graphic-properties element. Used in read_anchor()
void distToODF(const char * odfEl, const QString& emuValue);

//! Used by read_wrap*()
void readWrap();

bool m_drawing_anchor; //! set by read_drawing() to indicate if we have encountered drawing/anchor, used by read_pic()
bool m_drawing_inline; //! set by read_drawing() to indicate if we have encountered drawing/inline, used by read_pic()

// Shape properties
qint64 m_svgX; //!< set by read_off()
qint64 m_svgY; //!< set by read_off()
int m_svgWidth; //! set by read_ext()
int m_svgHeight; //! set by read_ext()
int m_svgChX; //!< set by read_chOff()
int m_svgChY; //!< set by read_chOff()
int m_svgChWidth; //! set by read_chExt()
int m_svgChHeight; //! set by read_chExt()
// These have to be in a vector in order to support group shapes within
// a group shape


bool m_inGrpSpPr; //Whether we are in group shape, affects transformations
struct GroupProp {
    qint64 svgXOld;
    qint64 svgYOld;
    qreal svgWidthOld;
    qreal svgHeightOld;
    qreal svgXChOld;
    qreal svgYChOld;
    qreal svgWidthChOld;
    qreal svgHeightChOld;
};
QVector<GroupProp> m_svgProp; //! value of the parent
bool m_flipH; //! set by read_xfrm()
bool m_flipV; //! set by read_xfrm()
int m_rot; //! set by read_xfrm()

QString m_xlinkHref; //!< set by read_blip()
QString m_cNvPrId; //!< set by read_cNvPr()
QString m_cNvPrName; //!< set by read_cNvPr()
QString m_cNvPrDescr; //!< set by read_cNvPr()

//! When dealing with colors there's no way to know what type of attribute
//! we are setting. While MSOOXML doesn't need to know the context in which a
//! color is used, ODF does need to know this.
enum ColorType {
    BackgroundColor,
    OutlineColor,
    TextColor,
    GradientColor
};


bool    m_hyperLink;
QString m_hyperLinkTarget;

QString m_recentDestName; // recent image

