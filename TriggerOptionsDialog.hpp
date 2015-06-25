#pragma once

#include <QDialog>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QFormLayout>
#include "TriggerOptions.hpp"

class TriggerOptionsDialog : public QDialog {
    Q_OBJECT

private:
    QByteArray savedGeometry;
    TriggerOptions currentOptions;

    QCheckBox *activeCheck;
    QComboBox *channelSelect;
    QDoubleSpinBox *voltageEdit;
    QComboBox *edgeSelect;
    QCheckBox *showCallsCheck;

    QFormLayout *formLayout;

public:
    TriggerOptionsDialog();

    TriggerOptions getOptions();
    void setOptions(TriggerOptions options);

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void closeEvent(QCloseEvent *event);

private:
    QLayout *createLayoutUI();
    void connectSignalsAndSlots();

    void setDialogSize(QLayout *layoutUI);

    void setWidgetsEnable(bool enable);

private slots:
    void handleActiveChanged(int state);
    void handleChannelChanged(int index);
    void handleVoltageChanged(double value);
    void handleEdgeChanged(int index);
    void handleShowCallsChanged(int state);

signals:
    void optionsChanged(TriggerOptions options);
};