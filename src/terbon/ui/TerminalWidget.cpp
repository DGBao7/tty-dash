#include <QtGui/QKeyEvent>
#include <QtGui/QFont>
#include <QtGui/QTextCursor>
#include <string>

#include "ui/TerminalWidget.h"
#include "ui/themes/Colors.h"
#include "ui/themes/Interface.h"
#include "theme/Theme.h"

namespace terbon {

TerminalWidget::TerminalWidget(QWidget* parent)
    : QPlainTextEdit(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent , false);
    setAutoFillBackground(false);
    viewport() -> setAttribute(Qt::WA_OpaquePaintEvent , false);
    viewport() -> setAutoFillBackground(false);

    setStyleSheet(ttydash::Theme::_terminal_stylesheet());
    setObjectName(QStringLiteral("terbonTerminal"));

    QFont font("Consolas");
    font.setPointSize(10);
    setFont(font);
    setCursorWidth(7);

    PrintLogo();

    PrintMaster();
}

QString TerminalWidget::GetCommand()
{
    QTextCursor cursor = textCursor();

    cursor.setPosition(promptPosition);

    cursor.movePosition(
        QTextCursor::End,
        QTextCursor::KeepAnchor
    );

    return cursor.selectedText();
}

void TerminalWidget::keyPressEvent(QKeyEvent* event)
{
    if (event -> key() == Qt::Key_C &&
    event -> modifiers() & Qt::ControlModifier)
    {
        if (running_process)
        {
            running_process -> kill();
            running_process = nullptr;
            Print("\nProcess terminated");
            NewPrompt();
        }

        return;
    }

    if (running_process != nullptr &&
        running_process->state() == QProcess::Running)
    {
        if (event->key() == Qt::Key_Return ||
            event->key() == Qt::Key_Enter)
        {
            running_process->write(
                process_input.toUtf8()
            );

            running_process->write("\n");

            process_input.clear();

            QTextCharFormat format;
            format.setForeground(Colors::master_text_color);

            QTextCursor cursor = textCursor();
            cursor.movePosition(QTextCursor::End);
            cursor.insertText("\n" , format);

            setTextCursor(cursor);

            // NewPrompt();

            return;
        }

        if (event->key() == Qt::Key_Backspace)
        {
            if (process_input.isEmpty())
            {
                return;
            }

            process_input.chop(1);

            QTextCursor cursor = textCursor();
            cursor.movePosition(QTextCursor::End);
            cursor.deletePreviousChar();

            setTextCursor(cursor);

            return;
        }

        if (!event->text().isEmpty())
        {
            process_input += event->text();

            QTextCursor cursor = textCursor();
            cursor.movePosition(QTextCursor::End);
            cursor.insertText(event->text());

            setTextCursor(cursor);
        }

        return;
    }

    if (event->key() == Qt::Key_Return ||
        event->key() == Qt::Key_Enter)
    {
        QString command = GetCommand();

        if (!command.isEmpty())
        {
            command_history.push_back(command);
        }

        history_index = -1;

        emit CommandEntered(command);

        PrintMaster();

        return;
    }

    if (event->key() == Qt::Key_Backspace)
    {
        if (textCursor().position() <= promptPosition)
        {
            return;
        }
    }

    if (event -> key() == Qt::Key_Up)
    {
        if (command_history.empty())
        {
            return;
        }

        if (history_index < static_cast<int>(command_history.size()) - 1)
        {
            history_index++;
        }

        QTextCursor cursor = textCursor();
        cursor.setPosition(promptPosition);
        cursor.movePosition(
            QTextCursor::End ,
            QTextCursor::KeepAnchor
        );
        cursor.removeSelectedText();

        cursor.insertText(
            command_history[
                command_history.size() - 1 - history_index
            ]
        );

        cursor.movePosition(QTextCursor::End);
        setTextCursor(cursor);

        return;
    }

    if (event -> key() == Qt::Key_Down)
    {
        if (command_history.empty())
        {
            return;
        }

        if (history_index > 0)
        {
            history_index--;

            QTextCursor cursor = textCursor();
            cursor.setPosition(promptPosition);
            cursor.movePosition(
                QTextCursor::End ,
                QTextCursor::KeepAnchor
            );
            cursor.removeSelectedText();
            cursor.insertText(
                command_history[
                    command_history.size() - 1 - history_index
                ]
            );
            cursor.movePosition(QTextCursor::End);
            setTextCursor(cursor);
        }
        else
        {
            history_index = -1;

            QTextCursor cursor = textCursor();
            cursor.setPosition(promptPosition);
            cursor.movePosition(
                QTextCursor::End ,
                QTextCursor::KeepAnchor
            );
            cursor.removeSelectedText();
            cursor.movePosition(QTextCursor::End);
            setTextCursor(cursor);
        }

        return;
    }

    QTextCursor cursor = textCursor();

    if (cursor.position() < promptPosition)
    {
        cursor.setPosition(promptPosition);
        setTextCursor(cursor);
    }

    QPlainTextEdit::keyPressEvent(event);
}

void TerminalWidget::Print(QString text)
{
    QTextCharFormat format;
    format.setForeground(Colors::output_color);

    QTextCursor cursor = textCursor();

    cursor.movePosition(QTextCursor::End);

    cursor.insertText("\n" + text , format);

    setTextCursor(cursor);
}

void TerminalWidget::ClearTerminal()
{
    clear();

    PrintLogo();
}

void TerminalWidget::SetRunningProcess(QProcess* process)
{
    running_process = process;
}

QProcess* TerminalWidget::GetRunningProcess()
{
    return running_process;
}

void TerminalWidget::NewPrompt()
{
    PrintMaster();
}

void TerminalWidget::PrintLogo()
{
    QTextCharFormat logoFormat;
    logoFormat.setForeground(Colors::logo_color);

    QTextCharFormat labelFormat;
    labelFormat.setForeground(Colors::logo_color);

    QTextCharFormat valueFormat;
    valueFormat.setForeground(Colors::values_color);

    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);

    for (int i = 0; i < 15; i++)
    {
        cursor.insertText(
            QString::fromUtf8(Interface::logo[i]) ,
            logoFormat
        );

        if (i < 9)
        {
            for (int j = 0; j < 20; j++)
            {
                cursor.insertText(" " , logoFormat);
            }

            cursor.insertText(
                Interface::labels[i] ,
                labelFormat
            );

            cursor.insertText(
                " : " ,
                labelFormat
            );

            cursor.insertText(
                Interface::values[i] ,
                valueFormat
            );
        }

        cursor.insertText("\n");
    }

    cursor.insertText("\n");

    setTextCursor(cursor);
}

void TerminalWidget::PrintMaster()
{
    QTextCharFormat format;
    format.setForeground(Colors::master_text_color);

    QTextCursor cursor = textCursor();
    cursor.insertText("\n");
    cursor.movePosition(QTextCursor::End);
    cursor.insertText("master$ " , format);

    setTextCursor(cursor);

    promptPosition = cursor.position();
}

}
