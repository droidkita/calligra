#ifndef KSPREAD_GLOBAL_H
#define KSPREAD_GLOBAL_H

#include <kiconloader.h>

#include "kspread_factory.h"

#include <qdatetime.h>
#include <kdebug.h>

class ElapsedTime
{
 public:
  ElapsedTime()
  {
    m_time.start();
  }

  ElapsedTime( QString const & name )
    : m_name( name )
  {
    m_time.start();
    kdDebug() << "*** (" << name << ")... Starting measuring... " << endl;
  }

  ~ElapsedTime()
  {
    uint milliSec = m_time.elapsed();
    uint min = (uint) ( milliSec / ( 1000 * 60 ) );
    milliSec -= ( min * 60 * 1000 );
    uint sec = (uint) ( milliSec / 1000 );
    milliSec -= sec * 1000;

    if ( m_name.isNull() )
      kdDebug() << "*** Elapsed time: " << min << " min, " << sec << " sec, " << milliSec << " msec" << endl;
    else
      kdDebug() << "*** (" << m_name << ") Elapsed time " << min << " min, " << sec << " sec, " << milliSec << " msec" << endl;
  }

 private:
  QTime   m_time;
  QString m_name;
};


#define KSBarIcon( x ) BarIcon( x, KSpreadFactory::global() )
#define colWidth 60.0
#define heightOfRow 20.0

/* Definition of maximal supported rows - please check kspread_util (columnName) and kspread_cluster also */
#define KS_rowMax 0x7FFF
/* Definition of maximal supported columns - please check kspread_util (columnName) and kspread_cluster also */
#define KS_colMax 0x7FFF

/* Definition of maximal supported columns/rows, which can be merged */
#define KS_spanMax 0xFFF

enum PasteMode { Normal, Text, Format, NoBorder, Comment, Result, NormalAndTranspose, TextAndTranspoase,
		 FormatAndTranspose, NoBorderAndTranspose };
enum Operation { OverWrite, Add, Mul, Sub, Div };
enum Conditional { None, Equal,Superior,Inferior,SuperiorEqual,InferiorEqual,Between,Different, DifferentTo};
enum Action { Stop, Warning, Information};
enum Allow { Allow_All, Allow_Number, Allow_Text,Allow_Time,Allow_Date,Allow_Integer,Allow_TextLength, Allow_List};
enum Series { Column,Row,Linear,Geometric };
namespace KSpread
{  // necessary due to QDock* enums (Werner)
  enum MoveTo { Bottom, Left, Top, Right, BottomFirst };
}
enum MethodOfCalc { SumOfNumber, Min, Max, Average, Count, NoneCalc};

enum FormatType {
    Generic_format=0,
    Number_format=1, Text_format=5, Money_format=10, Percentage_format=25,
    Scientific_format=30,
    ShortDate_format=35, TextDate_format=36, Time_format=50,
    SecondeTime_format=51,
    Time_format1=52, Time_format2=53, Time_format3=54, Time_format4=55,
    Time_format5=56, Time_format6=57, Time_format7=58, Time_format8=59,
    fraction_half=70,fraction_quarter=71,fraction_eighth=72,fraction_sixteenth=73,
    fraction_tenth=74,fraction_hundredth=75,fraction_one_digit=76,
    fraction_two_digits=77,fraction_three_digits=78,
    date_format1=200,date_format2=201,date_format3=202,date_format4=203,
    date_format5=204,date_format6=205,date_format7=206,date_format8=207,
    date_format9=208,date_format10=209,date_format11=210,date_format12=211,
    date_format13=212,date_format14=213,date_format15=214,date_format16=215,
    date_format17=216,date_format18=217,date_format19=218,date_format20=219,
    date_format21=220,date_format22=221,date_format23=222,date_format24=223,
    date_format25=224,date_format26=225,
    Custom_format = 300
};

#endif
