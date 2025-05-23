#ifndef RESTLINK_PARAMETERLIST_H
#define RESTLINK_PARAMETERLIST_H

#include <RestLink/global.h>
#include <RestLink/parameter.h>

#include <QtCore/qlist.h>
#include <QtCore/qvariant.h>

namespace RestLink {

template<typename P = Parameter>
class RESTLINK_EXPORT ParameterList : public QList<P>
{
public:
    ParameterList();
    ParameterList(const QList<P> &other);

    bool contains(const QString &name) const;
    QVariant value(const QString &name) const;
    QVariantList values(const QString &name) const;
    P parameter(const QString &name) const;
    QStringList parameterNames() const;

    void setParameter(const QString &name, const QVariant &value);
    void setParameter(const QString &name, const QVariantList &values);
    void setParameter(const P &param);

    void addParameter(const QString &name, const QVariant &value);
    void addParameter(const QString &name, const QVariantList &values);
    void addParameter(const P &param);

    void removeParameter(const QString &name);
    void removeParameter(const QString &name, const QVariant &value);
    void removeParameter(const QString &name, const QVariantList &values);

    QList<P> parameters() const;

private:
    P newParameter(const QString &name, const QVariantList &values) const;

    typename QList<P>::ConstIterator find(const QString &name, bool *found) const;
    typename QList<P>::Iterator find(const QString &name, bool *found);
};

template<typename P>
inline ParameterList<P>::ParameterList()
{
}

template<typename P>
inline ParameterList<P>::ParameterList(const QList<P> &other)
    : QList<P>(other)
{
}

template<typename P>
bool ParameterList<P>::contains(const QString &name) const
{
    bool found = false;
    find(name, &found);
    return found;
}

template<typename P>
QVariant ParameterList<P>::value(const QString &name) const
{
    bool found = false;
    auto it = find(name, &found);
    return (found && !it->values().isEmpty() ? it->values().first() : QVariant());
}

template<typename P>
QVariantList ParameterList<P>::values(const QString &name) const
{
    bool found = false;
    auto it = find(name, &found);
    return (found ? it->values() : QVariantList());
}

template<typename P>
P ParameterList<P>::parameter(const QString &name) const
{
    bool found = false;
    auto it = find(name, &found);
    return (found ? *it : P());
}

template<typename P>
QStringList ParameterList<P>::parameterNames() const
{
    QStringList names;
    std::transform(this->begin(), this->end(), std::back_inserter(names), [](const P &parameter) {
        return parameter.name();
    });
    return names;
}

template<typename P>
void ParameterList<P>::setParameter(const QString &name, const QVariant &value)
{
    setParameter(newParameter(name, {value}));
}

template<typename P>
void ParameterList<P>::setParameter(const QString &name, const QVariantList &values)
{
    setParameter(newParameter(name, values));
}

template<typename P>
void ParameterList<P>::setParameter(const P &param)
{
    bool found = false;
    auto it = find(param.name(), &found);

    if (found)
        it->setValues(param.values());
    else
        append(param);
}

template<typename P>
void ParameterList<P>::addParameter(const QString &name, const QVariant &value)
{
    addParameter(newParameter(name, {value}));
}

template<typename P>
void ParameterList<P>::addParameter(const QString &name, const QVariantList &values)
{
    addParameter(newParameter(name, values));
}

template<typename P>
void ParameterList<P>::addParameter(const P &param)
{
    bool found = false;
    auto it = find(param.name(), &found);

    if (found)
        it->addValue(param.values());
    else
        this->append(param);
}

template<typename P>
void ParameterList<P>::removeParameter(const QString &name)
{
    removeParameter(name, QVariantList());
}

template<typename P>
void ParameterList<P>::removeParameter(const QString &name, const QVariant &value)
{
    removeParameter(name, { value });
}

template<typename P>
void ParameterList<P>::removeParameter(const QString &name, const QVariantList &values)
{
    bool found = false;
    auto it = find(name, &found);

    if (found && (values.isEmpty() || it->values() == values))
        this->removeAt(std::distance(this->begin(), it));
}

template<typename P>
P ParameterList<P>::newParameter(const QString &name, const QVariantList &values) const
{
    P param;
    param.setName(name);
    param.setValues(values);
    return param;
}

template<typename P>
typename QList<P>::Iterator ParameterList<P>::find(const QString &name, bool *found)
{
    auto it = std::find_if(this->begin(), this->end(), [&name](const Parameter &param) {
        return param.name().compare(name, Qt::CaseInsensitive) == 0;
    });

    *found = (it != this->end() ? true : false);
    return it;
}

template<typename P>
typename QList<P>::ConstIterator ParameterList<P>::find(const QString &name, bool *found) const
{
    auto it = std::find_if(this->begin(), this->end(), [&name](const Parameter &param) {
        return param.name().compare(name, Qt::CaseInsensitive) == 0;
    });

    *found = (it != this->end() ? true : false);
    return it;
}

} // namespace RestLink

#endif // RESTLINK_PARAMETERLIST_H
