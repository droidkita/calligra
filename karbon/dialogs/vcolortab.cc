/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>

#include <kcolordialog.h>
#include <klocale.h>
#include <knuminput.h>

#include "vcolor.h"
#include "vfillcmd.h"
#include "vcolortab.h"
#include "vselection.h"

#include <kdebug.h>


VColorTab::VColorTab( const VColor &c, QWidget* parent, const char* name )
	: QTabWidget( parent, name )
{
	QGridLayout *mainLayout;

	mRGBWidget = new QWidget( this );
	mainLayout = new QGridLayout( mRGBWidget, 3, 3 );
	mColorSelector = new KHSSelector( mRGBWidget );
	mColorSelector->setMinimumHeight( 165 );
	mColorSelector->setMinimumWidth( 165 );
	connect( mColorSelector, SIGNAL( valueChanged( int, int ) ), this, SLOT( slotHSChanged( int, int ) ) );
	mainLayout->addMultiCellWidget(mColorSelector, 0, 2, 0, 0 );

	//Selector
	mSelector = new KGradientSelector( KSelector::Vertical, mRGBWidget );
	mSelector->setColors( QColor( "white" ), QColor( "black" ) );
	mSelector->setMinimumWidth( 12 );
	//TODO: Make it autochange color if the solid-filled object is selected (also for QSpinBoxes)
	connect( mSelector, SIGNAL( valueChanged( int ) ), this, SLOT( slotVChanged( int ) ) );
	mainLayout->addMultiCellWidget( mSelector, 0, 2, 1, 1 );

	//Reference
	QGroupBox* groupbox = new QGroupBox( 2, Vertical, i18n("Reference" ), mRGBWidget );
	new QLabel( i18n("Old:"), groupbox );
	new QLabel( i18n("New:"), groupbox );
	mOldColor = new KColorPatch( groupbox );
	mColorPreview = new KColorPatch( groupbox );
	QColor color( "black" );
	color = c.toQColor();

	mOldColor->setColor( color );
	mColorPreview->setColor( color );
	connect( mColorPreview, SIGNAL( colorChanged( QColor* ) ), this, SLOT( slotUpdate( QColor* ) ) );
	mainLayout->addWidget( groupbox, 0, 2 );

	//Components
	QGroupBox* cgroupbox = new QGroupBox( 3, Vertical, i18n("Components"), mRGBWidget );

	//--->RGB
	new QLabel( i18n("R:"), cgroupbox );
	new QLabel( i18n("G:"), cgroupbox );
	new QLabel( i18n("B:"), cgroupbox );
	mRed = new QSpinBox( 0, 255, 1, cgroupbox );
	mGreen = new QSpinBox( 0, 255, 1, cgroupbox );
	mBlue = new QSpinBox( 0, 255, 1, cgroupbox );
	connect( mRed, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromRGBSpinBoxes() ) );
	connect( mGreen, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromRGBSpinBoxes() ) );
	connect( mBlue, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromRGBSpinBoxes() ) );

	//--->HSV
	new QLabel( i18n("Hue:", "H:"), cgroupbox );
	new QLabel( i18n("Saturation:", "S:"), cgroupbox );
	new QLabel( i18n("Value:", "V:"), cgroupbox );
	mHue = new QSpinBox( 0, 359, 1, cgroupbox );
	mSaturation = new QSpinBox( 0, 255, 1, cgroupbox );
	mValue = new QSpinBox( 0, 255, 1, cgroupbox );
	connect( mHue, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromHSVSpinBoxes() ) );
	connect( mSaturation, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromHSVSpinBoxes() ) );
	connect( mValue, SIGNAL( valueChanged(int) ), this, SLOT( slotUpdateFromHSVSpinBoxes() ) );
	mainLayout->addWidget( cgroupbox, 1, 2 );

	//--->Opacity
	QGroupBox* ogroupBox = new QGroupBox( 1, Vertical, i18n("Opacity"), mRGBWidget );
	mOpacity = new KIntNumInput( 100, ogroupBox );
	mOpacity->setRange( 0, 100, 1, true );
	mOpacity->setValue( c.opacity() * 100.0 );
	mainLayout->addWidget( ogroupBox, 2, 2 );

	mainLayout->setSpacing( 2 );
	mainLayout->setMargin( 5 );

	mainLayout->activate();

	addTab( mRGBWidget, i18n("RGB") );

	mRed->setValue( color.red() );
	mGreen->setValue( color.green() );
	mBlue->setValue( color.blue() );
}

void VColorTab::slotUpdateFromRGBSpinBoxes()
{
	QColor color( mRed->value(), mGreen->value(), mBlue->value(), QColor::Rgb );
    mColorPreview->setColor( color );
	mColorPreview->update();

	// set HSV
	mHue->blockSignals( true );
	mSaturation->blockSignals( true );
	mValue->blockSignals( true );

	int h, s, v;
	color.hsv( &h, &s, &v );
	mHue->setValue( h );
	mSaturation->setValue( s );
	mValue->setValue( v );

	// update gradient selector
	mSelector->blockSignals( true );
	mColorSelector->setValues( h, s );
	slotHSChanged( h, s );
	mSelector->setValue( static_cast<int>( ( float( mValue->value() ) / 255.0 ) * 99.0 ) );
	mSelector->blockSignals( false );

	mHue->blockSignals( false );
	mSaturation->blockSignals( false );
	mValue->blockSignals( false );
}

void VColorTab::slotUpdateFromHSVSpinBoxes()
{
    QColor color( mHue->value(), mSaturation->value(), mValue->value(), QColor::Hsv );
    mColorPreview->setColor( color );
	mColorPreview->update();

	// update gradient selector
	mSelector->blockSignals( true );
	mSelector->setValue( static_cast<int>( ( float( mValue->value() ) / 255.0 ) * 99.0 ) );
	mSelector->blockSignals( false );

	// set RGB
	mRed->blockSignals( true );
	mGreen->blockSignals( true );
	mBlue->blockSignals( true );

	mRed->setValue( color.red() );
	mGreen->setValue( color.green() );
	mBlue->setValue( color.blue() );

	mRed->blockSignals( false );
	mGreen->blockSignals( false );
	mBlue->blockSignals( false );
}

VColor
VColorTab::getColor()
{
	kdDebug() << "VColorTab::slotApplyButtonPressed" << endl;
	VColor color;
	float r = mRed->value() / 255.0, g = mGreen->value() / 255.0, b = mBlue->value() / 255.0;
	float op = mOpacity->value() / 100.0;
	color.setValues( &r, &g, &b, 0L );
	color.setOpacity( op );

	return color;
}

void VColorTab::slotUpdate( QColor * )
{
	/*mRed->setValue( color->red() );
	mGreen->setValue( color->green() );
	mBlue->setValue( color->blue() );*/
}

void VColorTab::slotHSChanged( int h, int s )
{
	//QColor color( mHue->value(), mSaturation->value(), newVal, QColor::Hsv );
	mHue->setValue( h );
	mSaturation->setValue( s );
	QColor color1( h, s, 255, QColor::Hsv );
	QColor color2( h, s, 0, QColor::Hsv );
	mSelector->setColors( color1, color2 );
}

void VColorTab::slotVChanged( int newVal )
{
	//QColor color( mHue->value(), mSaturation->value(), newVal, QColor::Hsv );
	mValue->setValue( static_cast<int>( float( newVal ) / 99.0 * 255.0 ) );
}

#include "vcolortab.moc"

