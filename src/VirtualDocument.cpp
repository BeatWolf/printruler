#include "VirtualDocument.h"

VirtualDocument::VirtualDocument()
{
	list = new QList<Document *>;
	nup = 1;
	border = 0;
	margin = 0;
	seperateJobs = false;
	showDeleted = false;
	booklet = false;
}

VirtualDocument::~VirtualDocument()
{
	int i = 0;
	for(; i < list->size(); i++){
		Document *temp = list->at(i);
		delete temp;
	}
	
	delete list;
}

void VirtualDocument::addDocument(Document *document){
	list->append(document);
}

void VirtualDocument::removeDocument(int index){
	Document *temp = list->at(index);
	list->removeAt(index);
	temp->deleteFile();
	delete temp;
}

void VirtualDocument::moveUp(int index){
	if(index > 0){
		Document *temp = list->at(index);
		list->replace(index , list->at(index -1) );
		list->replace(index-1 , temp );
	}
}
void VirtualDocument::moveDown(int index){
	if(index < list->size()-1){
		Document *temp = list->at(index);
		list->replace(index , list->at(index +1) );
		list->replace(index+1 , temp );
	}
}

int VirtualDocument::size(){
	int size = 0;
	int i = 0;
	for(; i < list->size();i++){
		Document *temp = list->at(i);
		size += temp->getSize(showDeleted);
		if(seperateJobs && nup > 1 && temp->getSize(showDeleted) % nup > 0){
			size += nup - temp->getSize(showDeleted) % nup;
		}
	}
	
	return size;
}

int VirtualDocument::getPageCount(){
	bool temp = showDeleted;
	showDeleted = false;
	int realSize = size();
	showDeleted = temp;
	
	int tempVal =(int)( realSize / nup);
	
	if(realSize % nup != 0)
		tempVal++;
	return tempVal;
}

int VirtualDocument::realSize(){
	bool temp = showDeleted;
	showDeleted = true;
	int realSize = size();
	showDeleted = temp;
	return realSize;
}

int VirtualDocument::documents(){
	return list->size();
}

ExactPage *VirtualDocument::getExactPage(int index){
	int i = 0;
	for(; i < list->size();i++){
		Document *temp = list->at(i);
		if(temp->getSize(showDeleted) > index){
			ExactPage * page = new ExactPage();
			page->doc = temp;
			page->index = index;
			return page;
		}else if(seperateJobs && 
				temp->getSize(showDeleted) + (nup- temp->getSize(showDeleted) % nup) > index 
				&& nup > 1 && temp->getSize(showDeleted) % nup > 0){
			return NULL;
		}
		
		index -= temp->getSize(showDeleted);
		
		if(seperateJobs && nup > 1 && temp->getSize(showDeleted) % nup > 0){
			index -= nup - temp->getSize(showDeleted) % nup;
		}
	}
	//no page with that index has been found
	return NULL;
}

QPixmap VirtualDocument::getPage(int index){
	ExactPage * page = getExactPage(index);
	if(page){
		QPixmap pix = page->doc->getPage(page->index,showDeleted);
		delete page;
		return pix;
	}
	
	return NULL;
}

QPixmap VirtualDocument::getSmallPage(int index){
	ExactPage * page = getExactPage(index);
	if(page){
		QPixmap pix = page->doc->getSmallPage(page->index,showDeleted);
		delete page;
		return pix;
	}
	return NULL;
}

QPixmap VirtualDocument::getBigPage(int index){
	ExactPage * page = getExactPage(index);
	if(page){
		QPixmap pix = page->doc->getBigPage(page->index,showDeleted);
		delete page;
		return pix;
	}
	return NULL;
}

bool VirtualDocument::isDeleted(int index){
	ExactPage * page = getExactPage(index);
	if(page){
		bool del = page->doc->isDeleted(page->index);
		delete page;
		return del;
	}
	return false;
}

void VirtualDocument::deletePage(int index){
	ExactPage * page = getExactPage(index);
	if(page){
		page->doc->deletePage(page->index,showDeleted);
		delete page;
	}
}

void VirtualDocument::undeletePage(int index){
	ExactPage * page = getExactPage(index);
	if(page){
		page->doc->undeletePage(page->index,showDeleted);
		delete page;
	}
}

void VirtualDocument::addBlankPage(int index, bool before){
	if(!before)
		index--;
	ExactPage * page = getExactPage(index);
	if(page){
		if(!before)
			page->index++;
		
		page->doc->addBlankPage(page->index,showDeleted);
		delete page;
	}
}

void VirtualDocument::setNup(int pages){
	nup = pages;
}

int VirtualDocument::getNup(){
	return nup;
}

void VirtualDocument::setBorder(int style){
	border = style;
}

int VirtualDocument::getBorder(){
	return border;
}

void VirtualDocument::setMargin(int style){
	margin = style;
}

int VirtualDocument::getMargin(){
	return margin;
}

void VirtualDocument::setSeperateJobs(bool seperate){
	seperateJobs = seperate;
}

bool VirtualDocument::isSeperateJobs(){
	return seperateJobs;
}

void VirtualDocument::setShowDeleted(bool show){
	showDeleted = show;
}

bool VirtualDocument::isShowDeleted(){
	return showDeleted;
}

QString VirtualDocument::getPageSource(int i){
	return QString(*list->at(i)->getFile());
}

int VirtualDocument::getDocumentSize(int index){
	return list->at(index)->getSize(false);
}

int VirtualDocument::getRealIndex(int doc, int page){
	return list->at(doc)->getRealIndex(page);
}

bool VirtualDocument::isBooklet(){
	return booklet;
}

void VirtualDocument::setBooklet(bool book){
	booklet = book;
}
