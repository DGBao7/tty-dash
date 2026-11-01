#ifndef TERMINALWIDGET_H
#define TERMINALWIDGET_H

#include <QtWidgets/QPlainTextEdit>
#include <QtCore/QProcess>
#include <vector>

namespace terbon {


class TerminalWidget : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit TerminalWidget(QWidget* parent = nullptr);

    void Print(QString text);
    void ClearTerminal();

    void SetRunningProcess(QProcess* process);
    QProcess* GetRunningProcess();
    QString process_input;

    void NewPrompt();

signals:
    void CommandEntered(const QString& command);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    void PrintLogo();
    void PrintMaster();

    QString GetCommand();

    int promptPosition;

    std::vector<QString> command_history;
    int history_index = -1;

    QProcess* running_process = nullptr;
};

}

#endif
