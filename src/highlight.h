#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include <QObject>
#include <QSyntaxHighlighter>

struct HighlightPattern {
    QString key;
    bool caseSensitive{false};
    QColor color;

    //QJsonObject saveToJson();
    //void loadFromJson(QJsonObject o);
};

class Highlighter: public QSyntaxHighlighter
{
    Q_OBJECT
public:
    Highlighter(QTextDocument* doc);
protected:
    void highlightBlock(const QString &text) override;

public:
    void quickHighlight(const QString& text, QColor color = Qt::blue);
    void searchHighlight(const QString& text, bool caseSensitive);

    const QList<HighlightPattern> allQuickHighlights(){return mQuickHighLights.values();}

public:
   // QJsonObject saveToJson();
   // void loadFromJson(QJsonObject o);

signals:
    void onPatternAdded(HighlightPattern pattern);

public slots:
    void clearQuickHighlight(const QString& text);
    void clearSearchHighlight();
    void clearHighlight();

private:
    QMap<QString, HighlightPattern> mQuickHighLights;
    HighlightPattern mSearchHighLight;
};

#endif // HIGHLIGHTER_H
