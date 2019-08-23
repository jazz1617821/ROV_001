/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#pragma once

#include <QDoubleValidator>

class MyValidator : public QDoubleValidator
{
public:
	MyValidator(double bottom, double top, int decimals, QObject * parent) :
		QDoubleValidator(bottom, top, decimals, parent)
	{
	}

	QValidator::State validate(QString &s, int &i) const
	{
		if (s.isEmpty() || s == "-") {
			return QValidator::Intermediate;
		}

		QChar decimalPoint = locale().decimalPoint();

		if (s.indexOf(decimalPoint) != -1) {
			int charsAfterPoint = s.length() - s.indexOf(decimalPoint) - 1;

			if (charsAfterPoint > decimals()) {
				return QValidator::Invalid;
			}
		}

		bool ok;
		double d = locale().toDouble(s, &ok);

		if (ok && d >= bottom() && d <= top()) {
			return QValidator::Acceptable;
		}
		else {
			return QValidator::Invalid;
		}
	}
};
