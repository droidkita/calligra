/* This file is part of the KDE project

   Copyright 2018 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "ChartCreationWidget.h"

#include "ChartShape.h"
#include "ChartDebug.h"

using namespace KoChart;


ChartCreationWidget::ChartCreationWidget()
    : KoShapeConfigWidgetBase()
    , m_chart(0)
    , m_type(LastChartType)
    , m_subType(NoChartSubtype)
{
    m_ui.setupUi(this);

    connect(m_ui.chartBar, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartBarStacked, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartBarPercent, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartLine, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartLineStacked, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartLinePercent, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartArea, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartAreaStacked, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartAreaPercent, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartPie, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartRing, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartStockHLC, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartStockOHLC, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartStockCandleStick, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartRadar, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartRadarFilled, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartBubbles, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
    connect(m_ui.chartScatter, &QToolButton::toggled, this, &ChartCreationWidget::toggled);
}


ChartCreationWidget::~ChartCreationWidget()
{
}

bool ChartCreationWidget::showOnShapeCreate()
{
    return true;
}

void ChartCreationWidget::open(KoShape *shape)
{
    m_chart = dynamic_cast<ChartShape*>(shape);
}

void ChartCreationWidget::save()
{
    if (!m_chart) {
        return;
    }
    m_chart->setChartType(m_type);
    m_chart->setChartSubType(m_subType);
}

void ChartCreationWidget::toggled(bool value)
{
    if (!value) {
        return;
    }
    QToolButton *btn = qobject_cast<QToolButton*>(sender());
    if (!btn) {
        return;
    }
    if (btn == m_ui.chartBar) {
        m_type = BarChartType;
        m_subType = NormalChartSubtype;
    } else if (btn == m_ui.chartBarStacked) {
        m_type = BarChartType;
        m_subType = StackedChartSubtype;
    } else if (btn == m_ui.chartBarPercent) {
        m_type = BarChartType;
        m_subType = PercentChartSubtype;
    } else if (btn == m_ui.chartLine) {
        m_type = LineChartType;
        m_subType = NormalChartSubtype;
    } else if (btn == m_ui.chartLineStacked) {
        m_type = LineChartType;
        m_subType = StackedChartSubtype;
    } else if (btn == m_ui.chartLinePercent) {
        m_type = LineChartType;
        m_subType = PercentChartSubtype;
    } else if (btn == m_ui.chartArea) {
        m_type = AreaChartType;
        m_subType = NormalChartSubtype;
    } else if (btn == m_ui.chartAreaStacked) {
        m_type = AreaChartType;
        m_subType = NormalChartSubtype;
    } else if (btn == m_ui.chartAreaPercent) {
        m_type = AreaChartType;
        m_subType = PercentChartSubtype;
    } else if (btn == m_ui.chartPie) {
        m_type = CircleChartType;
        m_subType = NoChartSubtype;
    } else if (btn == m_ui.chartRing) {
        m_type = RingChartType;
        m_subType = NoChartSubtype;
    } else if (btn == m_ui.chartStockHLC) {
        m_type = StockChartType;
        m_subType = HighLowCloseChartSubtype;
    } else if (btn == m_ui.chartStockOHLC) {
        m_type = StockChartType;
        m_subType = OpenHighLowCloseChartSubtype;
    } else if (btn == m_ui.chartStockCandleStick) {
        m_type = StockChartType;
        m_subType = CandlestickChartSubtype;
    } else if (btn == m_ui.chartRadar) {
        m_type = RadarChartType;
        m_subType = NoChartSubtype;
    } else if (btn == m_ui.chartRadarFilled) {
        m_type = FilledRadarChartType;
        m_subType = NoChartSubtype;
    } else if (btn == m_ui.chartScatter) {
        m_type = ScatterChartType;
        m_subType = NoChartSubtype;
    } else if (btn == m_ui.chartBubbles) {
        m_type = BubbleChartType;
        m_subType = NoChartSubtype;
    } else {
        Q_ASSERT(true);
    }
}
