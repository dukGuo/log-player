
#include <QJsonObject>
#include <QJsonArray>
#include "highlight.h"

Highlighter::Highlighter(QTextDocument* doc): QSyntaxHighlighter(doc)
{
}

void Highlighter::highlightBlock(const QString &text) {
    bool searchHlApplied = false;

    //选词高亮
    for (auto p : mQuickHighLights) {
        int i = 0;
        auto len = p.key.length();
        QTextCharFormat fmt;
        fmt.setFontUnderline(true);
        fmt.setForeground(Qt::white);
        fmt.setBackground(p.color);

        if (!searchHlApplied && p.key == mSearchHighLight.key) {
            fmt.setFontUnderline(true);
            searchHlApplied=true;
        }

        while ((i = text.indexOf(p.key, i, p.caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive)) >= 0) {
            setFormat(i, len, fmt);
            i+=len;
        }
    }

   //搜索高亮
    if (mSearchHighLight.key.length() == 0 || searchHlApplied)
        return;

    auto len = mSearchHighLight.key.length();
    QTextCharFormat fmt;
    fmt.setFontUnderline(true);
    fmt.setBackground(QColor(Qt::gray));
    int i = 0;
    while ((i = text.indexOf(mSearchHighLight.key, i, mSearchHighLight.caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive)) >= 0) {
        setFormat(i, len, fmt);
        i+=len;
    }
}

void Highlighter::quickHighlight(const QString &text, QColor color) {
    if (mQuickHighLights.contains(text)) {
        if (mQuickHighLights[text].color != color) {
            mQuickHighLights[text].color = color;
            rehighlight();

        }

    }

    HighlightPattern pattern;
    pattern.key = text;
    pattern.color = color;
    pattern.caseSensitive = true;

    mQuickHighLights.insert(text, pattern);

    rehighlight();

    emit onPatternAdded(pattern);
}

void Highlighter::searchHighlight(const QString &text, bool caseSensitive)
{
    mSearchHighLight.key = text;
    mSearchHighLight.caseSensitive = caseSensitive;

    rehighlight();
}

//QJsonObject Highlighter::saveToJson()
//{
//    QJsonObject o;
//    o["searchHl"] = mSearchHighLight.saveToJson();
//    QJsonArray quickHls;
//    for (auto&& v : mQuickHighLights.values()) {
//        quickHls.append(v.saveToJson());
//    }
//    o["quickHls"] = quickHls;
//    return o;
//}

//void Highlighter::loadFromJson(QJsonObject o)
//{
//    mSearchHighLight.loadFromJson(o["searchHl"].toObject());
//    for (auto&& v : o["quickHls"].toArray()) {
//        HighlightPattern p;
//        p.loadFromJson(v.toObject());
//        mQuickHighLights[p.key] = p;
//    }
//}

void Highlighter::clearSearchHighlight()
{
    mSearchHighLight.key.clear();
    rehighlight();
}

void Highlighter::clearHighlight()
{
    mQuickHighLights.clear();
    mSearchHighLight.key.clear();
    rehighlight();
}

void Highlighter::clearQuickHighlight(const QString& text)
{
    mQuickHighLights.remove(text);
    rehighlight();
}


//QJsonObject HighlightPattern::saveToJson()
//{
//    QJsonObject o;
//    o["key"] = key;
//    o["caseSentive"] = caseSensitive;
//    o["color"] = color.name();
//    return o;
//}

//void HighlightPattern::loadFromJson(QJsonObject o)
//{
//    key = o["key"].toString();
//    caseSensitive = o["caseSentive"].toBool();
//    color = o["color"].toString();
//}
