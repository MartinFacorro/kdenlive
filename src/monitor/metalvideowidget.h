/*
    SPDX-FileCopyrightText: 2023 Meltytech, LLC
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "videowidget.h"

class MetalVideoRenderer;

class MetalVideoWidget : public VideoWidget
{
    Q_OBJECT
public:
    explicit MetalVideoWidget(int id, QObject *parent);
    virtual ~MetalVideoWidget();

public Q_SLOTS:
    void initialize() override;
    void renderVideo() override;

protected:
    void updateRulerHeight(int addedHeight) override;

private:
    std::unique_ptr<MetalVideoRenderer> m_renderer;
};
