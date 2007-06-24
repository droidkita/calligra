/* This file is part of the KDE project
 * Copyright (C) 2007 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "Renderer.h"
#include "MusicStyle.h"

#include "core/Sheet.h"
#include "core/Part.h"
#include "core/Voice.h"
#include "core/Staff.h"
#include "core/VoiceBar.h"
#include "core/Chord.h"
#include "core/Note.h"
#include "core/Clef.h"
#include "core/Bar.h"
#include "core/KeySignature.h"
#include "core/TimeSignature.h"

using namespace MusicCore;

MusicRenderer::MusicRenderer(MusicStyle* style) : m_style(style), m_debug(true)
{
}

void MusicRenderer::renderSheet(QPainter& painter, Sheet* sheet)
{
    for (int i = 0; i < sheet->partCount(); i++) {
        renderPart(painter, sheet->part(i));
    }
}

void MusicRenderer::renderPart(QPainter& painter, Part* part)
{
    for (int i = 0; i < part->staffCount(); i++) {
        renderStaff(painter, part->staff(i));
    }
    double firstStaff = part->staff(0)->top();
    int c = part->staffCount()-1;
    double lastStaff = part->staff(c)->bottom();
    for (int b = 0; b < part->sheet()->barCount(); b++) {
        Bar* bar = part->sheet()->bar(b);
        QPointF p = bar->position();
        painter.drawLine(QPointF(p.x() + bar->size(), p.y() + firstStaff), QPointF(p.x() + bar->size(), p.y() + lastStaff));
    }
    for (int i = 0; i < part->voiceCount(); i++) {
        renderVoice(painter, part->voice(i));
    }
}

void MusicRenderer::renderStaff(QPainter& painter, Staff *staff )
{
    double dy = staff->lineSpacing();
    double y = staff->top();
    for (int b = 0; b < staff->part()->sheet()->barCount(); b++) {
        Bar* bar = staff->part()->sheet()->bar(b);
        QPointF p = bar->position();
        painter.setPen(m_style->staffLinePen());
        for (int i = 0; i < staff->lineCount(); i++) {
            painter.drawLine(QPointF(p.x(), p.y() + y + i * dy), QPointF(p.x() + bar->size(), p.y() + y + i * dy));
        }
        RenderState state;
        for (int e = 0; e < bar->staffElementCount(staff); e++) {
            renderStaffElement(painter, bar->staffElement(staff, e), p.x(), p.y(), state, bar->scale());
        }
    }
}

void MusicRenderer::renderVoice(QPainter& painter, Voice *voice, const QColor& color)
{
    RenderState state;
    state.clef = 0;
    for (int b = 0; b < voice->part()->sheet()->barCount(); b++) {
        Bar* bar = voice->part()->sheet()->bar(b);
        QPointF p = bar->position();
        VoiceBar* vb = voice->bar(bar);
        for (int e = 0; e < vb->elementCount(); e++) {
            if (vb->element(e)->staff()) {
                state.clef = vb->element(e)->staff()->lastClefChange(b, 0);
            }
            renderElement(painter, vb->element(e), p.x(), p.y(), state, bar->scale(), color);
        }
    }
}

void MusicRenderer::renderElement(QPainter& painter, VoiceElement* me, double x, double y, RenderState& state, double xScale, const QColor& color)
{
    double top = y;
    if (me->staff()) top += me->staff()->top();
    if (m_debug) {
        painter.setPen(QPen(Qt::blue));
        painter.drawLine(QPointF(x + me->x() * xScale, top + me->y() - 20), QPointF(x + me->x() * xScale, top + me->y() + 20));
        painter.drawLine(QPointF(x + me->x() * xScale + me->width(), top + me->y() - 20), QPointF(x + me->x() * xScale + me->width(), top + me->y() + 20));
    }

    // TODO: make this less hacky
    Chord *c = dynamic_cast<Chord*>(me);
    if (c) renderChord(painter, c, QPointF(x, y), state, xScale, color);
    TimeSignature* ts = dynamic_cast<TimeSignature*>(me);
    if (ts) renderTimeSignature( painter, ts, x, xScale);
}

void MusicRenderer::renderStaffElement(QPainter& painter, MusicCore::StaffElement* se, double x, double y, RenderState& state, double xScale)
{
    double top = y;
    top += se->staff()->top();
    if (m_debug) {
        painter.setPen(QPen(Qt::blue));
        painter.drawLine(QPointF(x + se->x() * xScale, top + se->y() - 20), QPointF(x + se->x() * xScale, top + se->y() + 20));
        painter.drawLine(QPointF(x + se->x() * xScale + se->width(), top + se->y() - 20), QPointF(x + se->x() * xScale + se->width(), top + se->y() + 20));
    }

    Clef *cl = dynamic_cast<Clef*>(se);
    if (cl) renderClef(painter, cl, x, state, xScale);
    KeySignature *ks = dynamic_cast<KeySignature*>(se);
    if (ks) renderKeySignature(painter, ks, x, state, xScale);
}


void MusicRenderer::renderClef(QPainter& painter, Clef *c, double x, RenderState& state, double xScale)
{
    state.clef = c;
    Staff* s = c->staff();
    m_style->renderClef(painter, x + c->x() * xScale, s->top() + (s->lineCount() - c->line()) * s->lineSpacing(), c->shape());
}

void MusicRenderer::renderKeySignature(QPainter& painter, KeySignature* ks, double x, RenderState& state, double xScale)
{
    Staff * s = ks->staff();
    double curx = x + ks->x() * xScale;
    // draw sharps
    int idx = 3;
    for (int i = 0; i < 7; i++) {
        if (ks->accidentals(idx) > 0) {
            int line = 14;
            if (state.clef && state.clef->shape() == Clef::FClef) line = 4;
            if (state.clef) {
                line -= 2*state.clef->line();
            } else {
                line -= 4;
            }
            line = line - idx;
            while (line < 0) line += 7;
            while (line >= 6) line -= 7;

            m_style->renderAccidental( painter, curx, s->top() + line * s->lineSpacing() / 2, 1 );

            curx += 6;
        }
        idx = (idx + 4) % 7;
    }

    // draw flats
    idx = 6;
    for (int i = 0; i < 7; i++) {
        if (ks->accidentals(idx) < 0) {
            int line = 14;
            if (state.clef && state.clef->shape() == Clef::FClef) line = 4;
            if (state.clef) {
                line -= 2*state.clef->line();
            } else {
                line -= 4;
            }
            line = line - idx;
            while (line < 0) line += 7;
            while (line >= 6) line -= 7;

            m_style->renderAccidental( painter, curx, s->top() + line * s->lineSpacing() / 2, -1 );

            curx += 6;
        }
        idx = (idx + 3) % 7;
    }
}

void MusicRenderer::renderTimeSignature(QPainter& painter, TimeSignature* ts, double x, double xScale)
{
    Staff* s = ts->staff();
    double hh = 0.5 * (s->lineCount() - 1) * s->lineSpacing();
    m_style->renderTimeSignatureNumber( painter, x + ts->x() * xScale, s->top() + hh, ts->width(), ts->beats());
    m_style->renderTimeSignatureNumber( painter, x + ts->x() * xScale, s->top() + 2*hh, ts->width(), ts->beat());
}

void MusicRenderer::renderChord(QPainter& painter, Chord* chord, QPointF ref, RenderState& state, double xScale, const QColor& color)
{
    double x = chord->x() * xScale;
    if (chord->noteCount() == 0) { // a rest
        Staff *s = chord->staff();
        m_style->renderRest( painter, ref.x() + x, ref.y() + s->top() + (2 - (chord->duration() == Chord::Whole)) * s->lineSpacing(), chord->duration(), color );
        return;
    }
    Note *n = chord->note(0);
    Staff * s = n->staff();
    int line = 14;
    if (state.clef && state.clef->shape() == Clef::FClef) line = 4;
    if (state.clef) {
        line -= 2*state.clef->line();
    } else {
        line -= 4;
    }
    line = line - n->pitch();
    if (line > 9) { // lines under the bar
        painter.setPen(m_style->staffLinePen(color));
        for (int i = 10; i <= line; i+= 2) {
            double y = s->top() + i * s->lineSpacing() / 2;
            painter.drawLine(ref + QPointF(x - 4, y), ref + QPointF(x + 10, y));
        }
    } else if (line < -1) { // lines above the bar
        painter.setPen(m_style->staffLinePen(color));
        for (int i = -2; i >= line; i-= 2) {
            double y = s->top() + i * s->lineSpacing() / 2;
            painter.drawLine(ref + QPointF(x - 4, y), ref + QPointF(x + 10, y));
        }
    }

    double stemLen = -7;
    double stemX = x + 6;
    if (line < 4) { stemLen = 7; stemX = x; }
    painter.setPen(m_style->stemPen(color));
    painter.drawLine(ref + QPointF(stemX, chord->y() + s->top() + line * s->lineSpacing() / 2),
                     ref + QPointF(stemX, chord->y() + s->top() + (line + stemLen) * s->lineSpacing() / 2));
    m_style->renderNoteHead( painter, ref.x() + x, ref.y() + chord->y() + s->top() + line * s->lineSpacing() / 2, chord->duration(), color );
}

void MusicRenderer::renderNote(QPainter& painter, MusicCore::Chord::Duration duration, double x, double y, double stemLength, const QColor& color)
{
    m_style->renderNoteHead(painter, x, y, duration, color);

    if (duration <= MusicCore::Chord::Half) {
        painter.setPen(m_style->stemPen(color));
        painter.drawLine(QPointF(x+6, y - stemLength), QPointF(x+6, y));
    }
    if (duration <= MusicCore::Chord::Eighth) {
        m_style->renderNoteFlags(painter, x+6, y - stemLength, duration, color);
    }
}

