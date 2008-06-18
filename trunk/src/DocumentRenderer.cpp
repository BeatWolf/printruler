#include "DocumentRenderer.h"

DocumentRenderer::DocumentRenderer(QString &doc, bool wait = true)
{
	file = new QString(doc);
	m_wait = wait;
}

DocumentRenderer::~DocumentRenderer()
{
	delete file;
}

void DocumentRenderer::run(){
	Document *doc = new Document(file, m_wait);
	emit finished(doc);
}
