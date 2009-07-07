


QString foo()
{
	QCoreApplication::translate("Foo","XXX","YYY");
}

Foo::Foo()
{
	tr("CTOR");
}

void Foo::bar()
{
	tr("BAR");
}
