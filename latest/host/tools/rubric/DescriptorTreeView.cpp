///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////


#include "rubric_headers.h"
#include "DescriptorTreeView.h"
#include "DescriptorTreeViewItem.h"

//==============================================================================
void DescriptorTreeView::actionListenerCallback(const String &msg)
{
	if (msg.startsWith("diag"))
	{
	    notesText->setText(msg.fromFirstOccurrenceOf(msg, false, true));
	}
}

DescriptorTreeView::DescriptorTreeView(File* DescriptorFile)
{
    outStream = new MemoryOutputStream;

    // get info building-blocks from built-in file
    const String infoBlocksXmlString (BinaryData::info_blocks_xml);

    XmlDocument ib_parser (infoBlocksXmlString);
    infoBlocksXml = ib_parser.getDocumentElement()->getFirstChildElement();

    // populate the tree from file or from default
    if ( (DescriptorFile != 0) && (DescriptorFile->exists()) )
    {
        const String treeXmlString (DescriptorFile->loadFileAsString());
        XmlDocument parser (treeXmlString);
		treeXml = parser.getDocumentElement();
	    rootItem = new DescriptorTreeViewItem (treeXml, treeXml, infoBlocksXml, -1);
    }
    else
    {
        const String treeXmlString (BinaryData::musicsusd_xml);
        XmlDocument parser (treeXmlString);
	    treeXml = parser.getDocumentElement();
	    rootItem = new DescriptorTreeViewItem (treeXml, treeXml, infoBlocksXml, -1);
    }
    MessageManager::getInstance()->registerBroadcastListener(this);
    
    // add and configure the treeview
    addAndMakeVisible( treeView = new TreeView());
	treeView->setDefaultOpenness(true);
	treeView->setRootItem (rootItem);		
	treeView->addMouseListener(this, true);
    treeView->setBounds(10, 60, 600, treeView->getParentHeight()-70);
	rootItem->setLinesDrawnForSubItems(false);
	treeView->setIndentSize(20);
	rootItem->setOpen(true);

    // the hex output area
	addAndMakeVisible(hexText = new TextEditor(), -1);
    hexText->setBounds(20 + treeView->getWidth(), 60, treeView->getParentWidth()-treeView->getWidth()-30, treeView->getHeight()); //-200);
	hexText->setMultiLine(true);
	hexText->setScrollbarsShown(true);
	hexText->setReturnKeyStartsNewLine(true);
    Font font = hexText->getFont();
#if JUCE_MAC
    font.setTypefaceName("Monaco");
    font.setHeight(13.0f);
#else
    font.setTypefaceName("Lucida Console");
    font.setHeight(10.0f);
#endif
    hexText->applyFontToAllText(font);
	
	// refresh button
	addAndMakeVisible(refreshButton = new TextButton("Refresh", "Refresh descriptor tree and data struct views."));
	refreshButton->addListener(this);  

	// comments button
	addAndMakeVisible(commentsButton = new TextButton("On", "Comments in data struct."));
    commentsButton->setBounds(hexText->getX()+100, 20, 80, 25);
	commentsButton->addListener(this);
    commentsButton->setClickingTogglesState(true);
//    commentsButton->setToggleState(true, true);

	// comments label
	addAndMakeVisible(commentsLabel = new Label(String::empty, "Comments:"));
    commentsLabel->setBounds(hexText->getX(), 20, 80, 25);
        
	// bytes button
	addAndMakeVisible(bytesButton = new TextButton("Hex", "Hex or bytestream display"));
    bytesButton->setBounds(hexText->getX()+200, 20, 80, 25);
	bytesButton->addListener(this);
    bytesButton->setClickingTogglesState(true);
    bytesButton->setToggleState(true, true);
    bytesButton->setState(Button::buttonDown);

    // notes
	addAndMakeVisible(notesText = new TextEditor(), -1);
	notesText->setBounds(refreshButton->getRight() + 20, 20, treeView->getRight()-refreshButton->getRight()-40, 25);
	notesText->setMultiLine(false);
    notesText->setColour( TextEditor::backgroundColourId, Colours::darkseagreen);
    notesText->setTextToShowWhenEmpty("Add a brief description of the descriptor here.", Colours::blue);
    notesText->setText(treeXml->getStringAttribute("notes"));
    notesText->addListener(this);
    Font notesFont = notesText->getFont();
    notesFont.setTypefaceName("Arial");
    notesFont.setHeight(16.0f);
    notesText->applyFontToAllText(notesFont);
}

DescriptorTreeView::~DescriptorTreeView()
{
    deleteAllChildren();

    if (rootItem != 0)
        delete rootItem;

    if (treeXml != 0)
        delete treeXml;
        
    if (infoBlocksXml != 0)
        delete infoBlocksXml;

    if (outStream != 0)
        delete outStream;
}


void DescriptorTreeView::paint (Graphics& g)
{
    g.setColour (Colours::grey);
    g.drawRect (treeView->getX(), treeView->getY(),
                treeView->getWidth(), treeView->getHeight());
}

void DescriptorTreeView::buttonClicked (Button* button)
{
    if (button == refreshButton)
    {
        UpdateTree();
    }
    else if (button == commentsButton)
    {
        bAddComments = commentsButton->getToggleState();
        commentsButton->setButtonText(bAddComments ? "On" : "Off");
        if (!bHexStream)
        {
            bHexStream = true;
            bytesButton->setToggleState(true, false);
            bytesButton->setButtonText(bHexStream ? "Hex" : "Bytes");
        }
        UpdateTree();
    }
    else if (button == bytesButton)
    {
        bHexStream = bytesButton->getToggleState();
        bytesButton->setButtonText(bHexStream ? "Hex" : "Bytes");
        if (!bHexStream)
        {
            if (bAddComments)
            {
                bAddComments = false;
                commentsButton->setToggleState(false, false);
                commentsButton->setButtonText(bAddComments ? "On" : "Off");
           }
        }
        UpdateTree();
    }
}

void DescriptorTreeView::textEditorTextChanged(TextEditor & editor)
{
    return;
}

void DescriptorTreeView::textEditorEscapeKeyPressed(TextEditor & editor)
{
    return;
}

void DescriptorTreeView::textEditorReturnKeyPressed(TextEditor & editor)
{
    String notesStr = notesText->getText();
    if (notesStr.length()>128)
    {
        notesStr = notesStr.dropLastCharacters(notesStr.length() - 128);
        notesText->setText(notesStr, false);
    }
    treeXml->setAttribute("notes", notesText->getText());
}


void DescriptorTreeView::textEditorFocusLost(TextEditor & editor)
{
    String notesStr = notesText->getText();
    if (notesStr.length()>128)
    {
        notesStr = notesStr.dropLastCharacters(notesStr.length() - 128);
        notesText->setText(notesStr, false);
    }
    treeXml->setAttribute("notes", notesText->getText());
}

void DescriptorTreeView::UpdateTree()
{
    rootItem->setOpen(false);

    processDescriptorTree();
    scrubDeletedIbs(treeXml);

    rootItem->setOpen(true);
}

void DescriptorTreeView::CondenseTree()
{
    TreeViewItem * item;
    int i = 0;
    
    rootItem->setSelected(true, true);
    item = rootItem->getSubItem(2);                 // close all subitems in music_output_plug_status_area_ib
    if (item)
	{
		item->setOpen(true);
		for (i=0; i<item->getNumSubItems(); i++)
		{
			if (item->getSubItem(i))
			{
				item->getSubItem(i)->setOpen(false);
			}
		}
    }
	    
    item = rootItem->getSubItem(3);                 // close all subitems in routing_status_ib
    if (item)
	{
		item->setOpen(true);
		for (i=0; i<item->getNumSubItems(); i++)
		{
			if (item->getSubItem(i))
			{
				item->getSubItem(i)->setOpen(false);
			}
		}
	}
}

void DescriptorTreeView::ExpandTree()
{
    TreeViewItem * item;
    int nItems = 0;
    int i = 0;
    
    rootItem->setSelected(true, true);
    item = rootItem->getSubItem(2);                 // open all subitems in music_output_plug_status_area_ib
    if (item)
	{
		item->setOpen(true);
		nItems = item->getNumSubItems();
		for (i=0; i<nItems; i++)
		{
			if (item->getSubItem(i))
			{
				item->getSubItem(i)->setOpen(true);
			}
		}
	}
	  
    item = rootItem->getSubItem(3);                 // open all subitems in routing_status_ib
    if (item)
	{
		item->setOpen(true);
		nItems = item->getNumSubItems();
		for (i=0; i<nItems; i++)
		{
			if (item->getSubItem(i))
			{
				item->getSubItem(i)->setOpen(true);
			}
		}
	}
}

void DescriptorTreeView::resized()
{
    treeView->setBounds(10, 60, 600, treeView->getParentHeight()-70);
    hexText->setBounds(20 + treeView->getWidth(), 60, treeView->getParentWidth()-treeView->getWidth()-30, treeView->getHeight()); //-200);
    refreshButton->setBounds(treeView->getX(), 20, 80, 25);
    commentsButton->setBounds(hexText->getX()+100, 20, 80, 25);
    commentsLabel->setBounds(hexText->getX(), 20, 80, 25);
    bytesButton->setBounds(hexText->getX()+200, 20, 80, 25);
	notesText->setBounds(refreshButton->getRight() + 20, 20, treeView->getRight()-refreshButton->getRight()-40, 25);
	DescriptorTreeView::UpdateTree();
}

bool DescriptorTreeView::saveXmlTree(const String& fileName)
{
    processDescriptorTree();
    scrubDeletedIbs(treeXml);

    treeXml->writeToFile( File(fileName), String::empty );
    
    return true;
}

void DescriptorTreeView::scrubDeletedIbs(XmlElement *parentXmlElement)
{
    forEachXmlChildElement (*parentXmlElement, child)
    {
        if (child)
        {
            if (child->getStringAttribute("deleted").contains("true"))
            {
                parentXmlElement->removeChildElement (child, false);
            }
            else
            {
                scrubDeletedIbs(child);
            }
        }
    }
}

int DescriptorTreeView::processDescriptorTree()
{
    int lengthAccumulator = -1;
    
    XmlElement* musicSUStatusAreaIB = treeXml->getChildByName("music_subunit_status_area_ib");
//    XmlElement* musicOutputPlugStatusAreaIB = treeXml->getChildByName("music_output_plug_status_area_ib");
    XmlElement* routingStatusIB = treeXml->getChildByName("routing_status_ib");

    // go through the descriptor and update nested length attributes
    if (musicSUStatusAreaIB)
    {
        lengthAccumulator = GetMusicSUStatusAreaIBLength(musicSUStatusAreaIB);
        
        if (routingStatusIB)
            lengthAccumulator += GetRoutingStatusIBLength(routingStatusIB);

        treeXml->setAttribute("descriptor_length", lengthAccumulator);
    }
    
    // go through the tree and write the descriptor info blocks to the output stream
    if (lengthAccumulator > 0)
    {
        outStream->reset();
        outStream->writeText("[*[descriptor length\n\t// ", false, false);
        outStream->writeText( String::formatted("0x%04X", treeXml->getIntAttribute("descriptor_length")), false, false );
        outStream->writeText("]*]", false, false);

        writeMusicSUStatusAreaIB(musicSUStatusAreaIB);
        
        if (routingStatusIB)
            writeRoutingStatusIB(routingStatusIB);
    
        // write the output stream to the descriptor bytes view
        hexText->clear();
        hexText->setText((const char *)(outStream->getData()));
        
        String hexStr = hexText->getText();

        String outStr = "";

        if (bHexStream)
        {
            outStr.append("{", 1);
        }
        else
        {
            outStr.append(String::formatted("%04X", treeXml->getIntAttribute("descriptor_length")), 4);
        }
        
        if (!bAddComments)
        {
            if (bHexStream)
            {
                outStr.append("\n\t", 2);
            }
        }
           
        int n = 1;
        int nModulo = hexText->getWidth()/38;
        
        if (nModulo < 4) nModulo = 4;
        
        while (hexStr.length()>0)
        {
            // handle embedded comment
            if (hexStr.startsWith("[*["))
            {
                if (bAddComments)
                {
                    if (bHexStream)
                    {
                        outStr.append("\n\t// ", 5);
                    }
                }
                hexStr = hexStr.substring(3);
                if (bAddComments)
                {
                    outStr.append(hexStr.upToFirstOccurrenceOf("]*]", false, true ), 128);
                }
                hexStr = hexStr.fromFirstOccurrenceOf("]*]", false, true );
                if (bAddComments)
                {
                    if (bHexStream)
                    {
                        outStr.append("\n\t", 2);
                    }
                    n = 1;
                }
            }
            else
            {
                if (bHexStream)
                {
                    outStr.append("0x" + hexStr, 4);
                    outStr.append(", ", 4);
                }
                else
                {
                    outStr.append(hexStr, 2);
                }
                if (0 == (n%nModulo))
                {
                    if (bHexStream)
                    {
                        outStr.append("\n\t", 4);
                    }
                }
                hexStr = hexStr.substring(2);
                n++;
            }
        }
        if (bHexStream)
        {
            outStr.append("\n};\n", 4);
        }
        hexText->setText(outStr);
    }
    
    return (lengthAccumulator + 2);    // add 2 for length bytes
}
    
int DescriptorTreeView::GetMusicSUStatusAreaIBLength(XmlElement * musicSUStatusAreaIB)
{
    musicSUStatusAreaIB->setAttribute("primary_fields_len", 6);
    musicSUStatusAreaIB->setAttribute("compound_len", 10);
    return (10 + 2);    // add 2 for length bytes
}

int DescriptorTreeView::GetRoutingStatusIBLength(XmlElement * routingStatusAreaIB)
{
    int lengthAccumulator = 8;

    routingStatusAreaIB->setAttribute("primary_fields_len", 4);

    int numElements = routingStatusAreaIB->getNumChildElements();
    if (numElements > 0)
    {
        XmlElement* childElement = routingStatusAreaIB->getFirstChildElement();
        if ((childElement->getTagName() == "su_dst_plug_ib") || (childElement->getTagName() == "su_src_plug_ib"))
        {
            lengthAccumulator += GetSUPlugIBLength(childElement);
        }
        else if (childElement->getTagName() == "music_plug_ib")
        {
            lengthAccumulator += GetMusicPlugIBLength(childElement);
        }
        if (numElements > 1)
        {
            for (int i=1; i<numElements; i++)
            {
                childElement = childElement->getNextElement();
                if ((childElement->getTagName() == "su_dst_plug_ib") || (childElement->getTagName() == "su_src_plug_ib"))
                {
                    lengthAccumulator += GetSUPlugIBLength(childElement);
                }
                else if (childElement->getTagName() == "music_plug_ib")
                {
                    lengthAccumulator += GetMusicPlugIBLength(childElement);
                }
            }
        }
    }

    routingStatusAreaIB->setAttribute("compound_len", lengthAccumulator);
    
    return (lengthAccumulator + 2);    // add 2 for length bytes
}

int DescriptorTreeView::GetSUPlugIBLength(XmlElement * suPlugIB)
{
    int lengthAccumulator = 12;

    suPlugIB->setAttribute("primary_fields_len", 8);

    int numElements = suPlugIB->getNumChildElements();
    if (numElements > 0)
    {
        XmlElement* childElement = suPlugIB->getFirstChildElement();
        if (childElement->getTagName() == "cluster_ib")
        {
            lengthAccumulator += GetClusterIBLength(childElement);
        }
        if (numElements > 1)
        {
            for (int i=1; i<numElements; i++)
            {
                childElement = childElement->getNextElement();
                if (childElement->getTagName() == "cluster_ib")
                {
                    lengthAccumulator += GetClusterIBLength(childElement);
                }
            }
        }
    }

    XmlElement* nameIB = suPlugIB->getChildByName("name_ib");
    if (nameIB)
    {
        lengthAccumulator += GetNameIBLength(nameIB);
    }

    suPlugIB->setAttribute("compound_len", lengthAccumulator);
    
    return (lengthAccumulator + 2);    // add 2 for length bytes
}

int DescriptorTreeView::GetClusterIBLength(XmlElement * clusterIB)
{
    int lengthAccumulator = 3;
    int numElements = clusterIB->getNumChildElements();
    if (numElements > 0)
    {
        XmlElement* childElement = clusterIB->getFirstChildElement();
        if (childElement)
        {
            if (childElement->getTagName() == "signal")
            {
                lengthAccumulator += 4;
            }
        }
        if (numElements > 1)
        {
           for (int i=1; i<numElements; i++)
           {
                childElement = childElement->getNextElement();
                if (childElement)
                {
                    if (childElement->getTagName() == "signal")
                    {
                        lengthAccumulator += 4;
                    }
                }
            }
        }
    }   
    clusterIB->setAttribute("primary_fields_len", lengthAccumulator);
    lengthAccumulator += 2;
    
    XmlElement* nameIB = clusterIB->getChildByName("name_ib");
    if (nameIB)
    {
        lengthAccumulator += GetNameIBLength(nameIB);
    }
    lengthAccumulator += 2;
    clusterIB->setAttribute("compound_len", lengthAccumulator);
    
    return (lengthAccumulator + 2);    // add 2 for length bytes
}

int DescriptorTreeView::GetMusicPlugIBLength(XmlElement * musicPlugIB)
{
    int lengthAccumulator = 14;

    musicPlugIB->setAttribute("primary_fields_len", lengthAccumulator);
    
    lengthAccumulator += 4;

    XmlElement* nameIB = musicPlugIB->getChildByName("name_ib");
    if (nameIB)
    {
        lengthAccumulator += GetNameIBLength(nameIB);
    }
    musicPlugIB->setAttribute("compound_len", lengthAccumulator);
   
    return (lengthAccumulator + 2);    // add 2 for length bytes
}

int DescriptorTreeView::GetNameIBLength(XmlElement * nameIB)
{
    XmlElement* nameData = nameIB->getChildByName("name_data");
    int lengthAccumulator = 0;

    if (nameData)
    {
        XmlElement* rawTextIB = nameData->getChildByName("raw_text_ib");
        if (rawTextIB)
        {
            String nameStr = rawTextIB->getStringAttribute("raw_text_data");
            if (nameStr.isEmpty())
            {
                nameStr = "name";
                rawTextIB->setAttribute("raw_text_data", nameStr);
            }
            
            // add one for text terminator, added by output formatter
            lengthAccumulator = nameStr.length() + 1;

            // output formatter will replace commas with 0x0D0A line separators, so add 1 for each comma
            while (nameStr.containsChar(','))
            {
               lengthAccumulator += 1; // net plus one character
               nameStr = nameStr.fromFirstOccurrenceOf(",", false, true);
            }
            
            rawTextIB->setAttribute("primary_fields_len", lengthAccumulator);
            lengthAccumulator += 4;                                             // add 2 for num_chars, 2 for ib_type
            rawTextIB->setAttribute("compound_len", lengthAccumulator);
            
            lengthAccumulator += 6;                                             // add 2 for compound_len, 2 for max_num_chars, 1 for name_data_attr, 1 for name_data_ref_type
            nameIB->setAttribute("primary_fields_len", lengthAccumulator);
            lengthAccumulator += 4;                                             // add 2 for ib_type, 2 for primary_fields_len
            nameIB->setAttribute("compound_len", lengthAccumulator);
        }
    }
    
    return (lengthAccumulator + 2);    // add 2 for length bytes
}
   
//=========================================================================================
void DescriptorTreeView::writeMusicSUStatusAreaIB(XmlElement * musicSUStatusAreaIB)
{
    outStream->writeText("[*[****************************************]*]", false, false);
    outStream->writeText("[*[music status area ib]*]", false, false);
    outStream->writeText( String::formatted("%04X", musicSUStatusAreaIB->getIntAttribute("compound_len")), false, false );
    String xmlStr = musicSUStatusAreaIB->getStringAttribute("ib_type").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    outStream->writeText( String::formatted("%04X", musicSUStatusAreaIB->getIntAttribute("primary_fields_len")), false, false );
    xmlStr = musicSUStatusAreaIB->getStringAttribute("current_tx_cap").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    xmlStr = musicSUStatusAreaIB->getStringAttribute("current_rx_cap").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    xmlStr = musicSUStatusAreaIB->getStringAttribute("current_latency_cap").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
}

void DescriptorTreeView::writeRoutingStatusIB(XmlElement * routingStatusAreaIB)
{
    outStream->writeText("[*[****************************************]*]", false, false);
    outStream->writeText("[*[routing status area ib]*]", false, false);
    outStream->writeText( String::formatted("%04X", routingStatusAreaIB->getIntAttribute("compound_len")), false, false );
    String xmlStr = routingStatusAreaIB->getStringAttribute("ib_type").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    outStream->writeText( String::formatted("%04X", routingStatusAreaIB->getIntAttribute("primary_fields_len")), false, false );
    outStream->writeText( String::formatted("%02X", routingStatusAreaIB->getIntAttribute("num_su_dst_plugs")), false, false );
    outStream->writeText( String::formatted("%02X", routingStatusAreaIB->getIntAttribute("num_su_src_plugs")), false, false );
    outStream->writeText( String::formatted("%04X", routingStatusAreaIB->getIntAttribute("num_music_plugs")), false, false );

    int numElements = routingStatusAreaIB->getNumChildElements();
    if (numElements > 0)
    {
        XmlElement* childElement = routingStatusAreaIB->getFirstChildElement();
        if (childElement->getTagName() == "su_dst_plug_ib")
        {
            outStream->writeText("[*[su dst plug ib]*]", false, false);
            writeSUPlugIB(childElement);
        }
        else if (childElement->getTagName() == "su_src_plug_ib")
        {
            outStream->writeText("[*[su src plug ib]*]", false, false);
            writeSUPlugIB(childElement);
        }
        else if (childElement->getTagName() == "music_plug_ib")
        {
            writeMusicPlugIB(childElement);
        }
        if (numElements > 1)
        {
            for (int i=1; i<numElements; i++)
            {
                childElement = childElement->getNextElement();
                if (childElement->getTagName() == "su_dst_plug_ib")
                {
                    outStream->writeText("[*[su dst plug ib]*]", false, false);
                    writeSUPlugIB(childElement);
                }
                else if (childElement->getTagName() == "su_src_plug_ib")
                {
                    outStream->writeText("[*[su src plug ib]*]", false, false);
                    writeSUPlugIB(childElement);
                }
                else if (childElement->getTagName() == "music_plug_ib")
                {
                    writeMusicPlugIB(childElement);
                }
            }
        }
    }
}

void DescriptorTreeView::writeSUPlugIB(XmlElement * suPlugIB)
{
    outStream->writeText( String::formatted("%04X", suPlugIB->getIntAttribute("compound_len")), false, false );
    String xmlStr = suPlugIB->getStringAttribute("ib_type").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    outStream->writeText( String::formatted("%04X", suPlugIB->getIntAttribute("primary_fields_len")), false, false );
    outStream->writeText( String::formatted("%02X", suPlugIB->getIntAttribute("su_plug_id")), false, false );
    xmlStr = suPlugIB->getStringAttribute("signal_format").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    xmlStr = suPlugIB->getStringAttribute("plug_type").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    outStream->writeText( String::formatted("%04X", suPlugIB->getIntAttribute("num_clusters")), false, false );
    outStream->writeText( String::formatted("%04X", suPlugIB->getIntAttribute("num_channels")), false, false );

    int numElements = suPlugIB->getNumChildElements();
    if (numElements > 0)
    {
        XmlElement* childElement = suPlugIB->getFirstChildElement();
        if (childElement->getTagName() == "cluster_ib")
        {
            writeClusterIB(childElement);
        }
        else if (childElement->getTagName() == "name_ib")
        {
            writeNameIB(childElement);
        }

        if (numElements > 1)
        {
            for (int i=1; i<numElements; i++)
            {
                childElement = childElement->getNextElement();
                if (childElement->getTagName() == "cluster_ib")
                {
                    writeClusterIB(childElement);
                }
                else if (childElement->getTagName() == "name_ib")
                {
                    writeNameIB(childElement);
                }
            }
        }
    }
}

void DescriptorTreeView::writeClusterIB(XmlElement * clusterIB)
{
    outStream->writeText("[*[cluster ib]*]", false, false);
    outStream->writeText( String::formatted("%04X", clusterIB->getIntAttribute("compound_len")), false, false );
    String xmlStr = clusterIB->getStringAttribute("ib_type").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    outStream->writeText( String::formatted("%04X", clusterIB->getIntAttribute("primary_fields_len")), false, false );
    xmlStr = clusterIB->getStringAttribute("stream_format").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    xmlStr = clusterIB->getStringAttribute("port_type").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    outStream->writeText( String::formatted("%02X", clusterIB->getIntAttribute("num_signals")), false, false );

    int numElements = clusterIB->getNumChildElements();
    if (numElements > 0)
    {
        XmlElement* childElement = clusterIB->getFirstChildElement();
        if (childElement)
        {
            if (childElement->getTagName() == "signal")
            {
                writeSignalIB(childElement);
            }
            else if (childElement->getTagName() == "name_ib")
            {
                writeNameIB(childElement);
            }
        }
        if (numElements > 1)
        {
           for (int i=1; i<numElements; i++)
           {
                childElement = childElement->getNextElement();
                if (childElement)
                {
                    if (childElement->getTagName() == "signal")
                    {
                        writeSignalIB(childElement);
                    }
                    else if (childElement->getTagName() == "name_ib")
                    {
                        writeNameIB(childElement);
                    }
                }
            }
        }
    }   
}

void DescriptorTreeView::writeSignalIB(XmlElement * signalIB)
{
    outStream->writeText("[*[signal]*]", false, false);
    outStream->writeText( String::formatted("%04X", signalIB->getIntAttribute("music_plug_id")), false, false );
    outStream->writeText( String::formatted("%02X", signalIB->getIntAttribute("stream_position")), false, false );
    outStream->writeText( String::formatted("%02X", signalIB->getIntAttribute("stream_location")), false, false );
}
void DescriptorTreeView::writeMusicPlugIB(XmlElement * musicPlugIB)
{
    outStream->writeText("[*[music plug ib]*]", false, false);
    outStream->writeText( String::formatted("%04X", musicPlugIB->getIntAttribute("compound_len")), false, false );
    String xmlStr = musicPlugIB->getStringAttribute("ib_type").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    outStream->writeText( String::formatted("%04X", musicPlugIB->getIntAttribute("primary_fields_len")), false, false );
    xmlStr = musicPlugIB->getStringAttribute("music_plug_type").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    outStream->writeText( String::formatted("%04X", musicPlugIB->getIntAttribute("music_plug_id")), false, false );
    xmlStr = musicPlugIB->getStringAttribute("routing_support").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    xmlStr = musicPlugIB->getStringAttribute("src_plug_fn_type").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    outStream->writeText( String::formatted("%02X", musicPlugIB->getIntAttribute("src_plug_id")), false, false );
    xmlStr = musicPlugIB->getStringAttribute("src_plug_fn_block_id").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    outStream->writeText( String::formatted("%02X", musicPlugIB->getIntAttribute("src_stream_position")), false, false );
    xmlStr = musicPlugIB->getStringAttribute("src_stream_location").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    xmlStr = musicPlugIB->getStringAttribute("dst_plug_fn_type").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    outStream->writeText( String::formatted("%02X", musicPlugIB->getIntAttribute("dst_plug_id")), false, false );
    xmlStr = musicPlugIB->getStringAttribute("dst_plug_fn_block_id").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );
    outStream->writeText( String::formatted("%02X", musicPlugIB->getIntAttribute("dst_stream_position")), false, false );
    xmlStr = musicPlugIB->getStringAttribute("dst_stream_location").fromFirstOccurrenceOf("0x", false, true);
    outStream->writeText( xmlStr, false, false );

    XmlElement* nameIB = musicPlugIB->getChildByName("name_ib");
    if (nameIB)
    {
        writeNameIB(nameIB);
    }
}

void DescriptorTreeView::writeNameIB(XmlElement * nameIB)
{
    outStream->writeText("[*[**name ib ]*]", false, false);
    outStream->writeText( String::formatted("%04X", nameIB->getIntAttribute("compound_len")), false, false );
//    String xmlStr = nameIB->getStringAttribute("ib_type").fromFirstOccurrenceOf("0x", false, true);
    String xmlStr = "000B";
    outStream->writeText( xmlStr, false, false );
    outStream->writeText( String::formatted("%04X", nameIB->getIntAttribute("primary_fields_len")), false, false );
    outStream->writeText( String::formatted("%02X", nameIB->getIntAttribute("name_data_ref_type")), false, false );

    XmlElement* nameData = nameIB->getChildByName("name_data");
    if (nameData)
    {
        outStream->writeText("[*[****name data]*]", false, false);
        outStream->writeText( String::formatted("%02X", nameData->getIntAttribute("name_data_attr")), false, false );
        xmlStr = nameData->getStringAttribute("max_num_chars").fromFirstOccurrenceOf("0x", false, true);
        outStream->writeText( xmlStr, false, false );

        XmlElement* rawTextIB = nameData->getChildByName("raw_text_ib");
        if (rawTextIB)
        {
            outStream->writeText("[*[******raw text ib]*]", false, false);
            outStream->writeText( String::formatted("%04X", rawTextIB->getIntAttribute("compound_len")), false, false );
            // xmlStr = rawTextIB->getStringAttribute("ib_type").fromFirstOccurrenceOf("0x", false, true);
            xmlStr = "000A";
            outStream->writeText( xmlStr, false, false );
            outStream->writeText( String::formatted("%04X", rawTextIB->getIntAttribute("primary_fields_len")), false, false );
            String nameStr = rawTextIB->getStringAttribute("raw_text_data");
            String outNameStr = String::empty;
            char buff[256];
            int i = 0;
            
            nameStr.copyToUTF8(buff,255);
            
            while (buff[i] != '\0')
            {
                // handle comma-separated string of labels
                if (buff[i] == ',')
                {
                    outNameStr.append("0D0A", 8);
                }
                else
                {
                    outNameStr.append(String::toHexString((int)buff[i]), 4);
                }
                i++;
            }
            outNameStr.append("00", 4);
            
            outStream->writeText(outNameStr, false, false);
            String strTemp = "[*[='" + nameStr + "']*]";
            outStream->writeText(strTemp, false, false);
        }
    }
}

//==============================================================================
DescriptorTreeView* createDescriptorTreeView(File* FileHandle)
{
    return new DescriptorTreeView(FileHandle);
}
    
