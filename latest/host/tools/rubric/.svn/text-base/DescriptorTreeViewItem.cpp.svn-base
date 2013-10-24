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
#include "MainWindow.h"

//==============================================================================
void DescriptorTreeViewItem::getTextEditorText(TextEditor & editor)
{
    String editorText = editor.getText();

    if (editorText.isEmpty())
    {
        editor.setText(xml->getAttributeName(attributeIndex) + "=name", true);
    }
    else if (editorText.containsChar('='))
    {
        String valueText = editorText.fromFirstOccurrenceOf("=", false, false);
     
        if (valueText.isEmpty())
        {
            editor.setText(xml->getAttributeName(attributeIndex) + "=name", true);
        }
        else
        {
            xml->setAttribute(xml->getAttributeName(attributeIndex), valueText);
            editor.setText(xml->getAttributeName(attributeIndex) + "=" + valueText, false);
        }
    }
    else
    {
        xml->setAttribute(xml->getAttributeName(attributeIndex), editorText);
        editor.setText(xml->getAttributeName(attributeIndex) + "=" + editorText, false);
    }
}

DescriptorTreeViewItem::DescriptorTreeViewItem (XmlElement* const rootXml_, XmlElement* const xml_, XmlElement* const ib_xml_, int attributeIndex_ )
        : rootXml (rootXml_), xml (xml_), ib_xml (ib_xml_), attributeIndex( attributeIndex_ )
{
}

DescriptorTreeViewItem::~DescriptorTreeViewItem()
{
}

void DescriptorTreeViewItem::actionListenerCallback(const String &msg)
{
}

int DescriptorTreeViewItem::getItemWidth() const
{
    return (-1);
}
    
String DescriptorTreeViewItem::getUniqueName() const
{
    if (xml != 0)
        return xml->getTagName();
    else
        return String::empty;
}

bool DescriptorTreeViewItem::mightContainSubItems()
{
    return (attributeIndex < 0) && xml && ((xml->getFirstChildElement() != 0) || (xml->getNumAttributes() > 0));
}

void DescriptorTreeViewItem::paintItem (Graphics& g, int width, int height)
{
    return;
}

void DescriptorTreeViewItem::itemOpennessChanged (bool isNowOpen)
{
	String xmlStr, openStr;
    XmlElement* child;
    
	if ( !xml )
        return;
    
    if ( attributeIndex >= 0 ) 
        return;
    
    if(isNowOpen)
    {
        if (xml->getStringAttribute("added").contains("true"))
        {
            clearSubItems();
            xml->removeAttribute("added");
        }
        if (getNumSubItems() == 0)  // haven't created the branch yet
        {
            // add attribute nodes, with filters
            for( int i=0; i<xml->getNumAttributes(); i++ )
            {
                if ( !( xml->getAttributeName(i).startsWith("xmlns")
	                ||  xml->getAttributeName(i).startsWith("xsi")
	                ||  xml->getAttributeName(i).startsWith("version")
	                ||  xml->getAttributeName(i).startsWith("openness")
	                ||  xml->getAttributeName(i).startsWith("notes") ) )
                {
                    addSubItem ( new DescriptorTreeViewItem (rootXml, xml, ib_xml, i));
                }
            }
            child = xml->getFirstChildElement();
            while ( child != 0 )
            {
                if (!child->getStringAttribute("deleted").contains("true") )
                {
                    addSubItem( new DescriptorTreeViewItem(rootXml, child, ib_xml, -1) );
                    child = child->getNextElement();
                }
                if (xml->getTagName().contains("plug_ib") || xml->getTagName().contains("name_ib"))
                {
                    setOpen(false);
                }
            }
        }
        else
        {
            int index = 0;
            for( int i=0; i<xml->getNumAttributes(); i++ )
            {
                if ( !( xml->getAttributeName(i).startsWith("xmlns")
	                ||  xml->getAttributeName(i).startsWith("xsi")
	                ||  xml->getAttributeName(i).startsWith("version")
	                ||  xml->getAttributeName(i).startsWith("openness")
	                ||  xml->getAttributeName(i).startsWith("notes") ) )
                {
                    index++;
                }
            }
            index=6;
            child = xml->getFirstChildElement();
            while ( child != 0 )
            {
                if (child->getStringAttribute("deleted").contains("true"))
                {
                    removeSubItem(index, true);
//                    child->removeAttribute("deleted");
                }
                child = child->getNextElement();
                index++;
            }
        }
    }
}

Component* DescriptorTreeViewItem::createItemComponent( void )
{
   if(attributeIndex >= 0 ) // an attribute
    {
		String attribute = xml->getAttributeName(attributeIndex);
		String value = xml->getAttributeValue(attributeIndex);
		
        if (attribute.startsWith("num_") 
	     || attribute.startsWith("src_plug_num") 
	     || attribute.startsWith("music_plug_id")
	     || attribute.startsWith("su_plug_id")
	     || attribute.startsWith("stream_position")
	     || attribute.startsWith("src_stream_position")
	     || attribute.startsWith("dst_stream_position") )
	    {
	        valueSlider = new Slider(attribute);
			valueSlider->setTooltip("type a value in the edit box, or drag the slider");
			valueSlider->setTextBoxStyle (Slider::TextBoxLeft, false, 200, 20);
            valueSlider->setRange(0, 96, 1);
            valueSlider->setColour(Slider::backgroundColourId, Colours::darkseagreen);
            valueSlider->setSliderStyle (Slider::LinearHorizontal);
            valueSlider->addListener(this);
            valueSlider->setTextValueSuffix ("    " + attribute);
            valueSlider->setChangeNotificationOnlyOnRelease(true);
            valueSlider->setValue(value.getIntValue(), false, false);
            return valueSlider;
	    }
	    else if (attribute.startsWith("subunit_plug_id"))
	    {
	        valueSlider = new Slider(attribute);
			valueSlider->setTooltip("type a value in the edit box, or drag the slider");
			valueSlider->setTextBoxStyle (Slider::TextBoxLeft, false, 200, 20);
            valueSlider->setRange(0, 30, 1);
            valueSlider->setColour(Slider::backgroundColourId, Colours::darkseagreen);
            valueSlider->setSliderStyle (Slider::LinearHorizontal);
            valueSlider->addListener(this);
            valueSlider->setTextValueSuffix ("    " + attribute);
            valueSlider->setChangeNotificationOnlyOnRelease(true);
            valueSlider->setValue(value.getIntValue(), false, false);
            return valueSlider;
	    }
	    else if (attribute.startsWith("src_plug_id") || attribute.startsWith("dst_plug_id") )
	    {
	        valueSlider = new Slider(attribute);
			valueSlider->setTooltip("type a value in the edit box, or drag the slider");
			valueSlider->setTextBoxStyle (Slider::TextBoxLeft, false, 200, 20);
            valueSlider->setRange(0, 255, 1);
            valueSlider->setColour(Slider::backgroundColourId, Colours::darkseagreen);
            valueSlider->setSliderStyle (Slider::LinearHorizontal);
            valueSlider->addListener(this);
            valueSlider->setTextValueSuffix ("    " + attribute);
            valueSlider->setChangeNotificationOnlyOnRelease(true);
            valueSlider->setValue(value.getIntValue(), false, false);
            return valueSlider;
	    }
		else if (attribute.startsWith("current_rx_cap") || attribute.startsWith("current_tx_cap"))
		{
			itemCombo = new ComboBox (attribute);
			itemCombo->setTooltip("click to select a setting");
			itemCombo->setEditableText (false);
			itemCombo->setJustificationType (Justification::left);
			itemCombo->setColour(ComboBox::backgroundColourId, Colours::darkseagreen);
			itemCombo->addItem (attribute + "    0x01     Non-Blocking", 1);
			itemCombo->addItem (attribute + "    0x02     Blocking", 2);
			itemCombo->addListener(this);
			value = value.substring(2);
			itemCombo->setSelectedId (value.getHexValue32(), true);
			return itemCombo;
		}
		else if (attribute.startsWith("audio_SYNC_activity"))
		{
			itemCombo = new ComboBox (attribute);
			itemCombo->setTooltip("click to select a setting");
			itemCombo->setEditableText (false);
			itemCombo->setJustificationType (Justification::left);
			itemCombo->setColour(ComboBox::backgroundColourId, Colours::darkseagreen);
			itemCombo->addItem (attribute + "    0x01     Bus", 1);
			itemCombo->addItem (attribute + "    0x02     Ex", 2);
			itemCombo->addListener(this);
			value = value.substring(2);
			itemCombo->setSelectedId (value.getHexValue32(), true);
			return itemCombo;
		}
		else if (attribute.startsWith("stream_format"))
		{
			itemCombo = new ComboBox (attribute);
			itemCombo->setTooltip("click to select a setting");
			itemCombo->setEditableText (false);
			itemCombo->setJustificationType (Justification::left);
			itemCombo->setColour(ComboBox::backgroundColourId, Colours::darkseagreen);
			itemCombo->addItem (attribute + "    0x00     IEC60958-3 SPDIF transparent", 1);
			itemCombo->addItem (attribute + "    0x01     IEC61937-3 AC3", 2);
			itemCombo->addItem (attribute + "    0x02     IEC61937-4 MPEG", 3);
			itemCombo->addItem (attribute + "    0x03     IEC61937-5 DTS", 4);
			itemCombo->addItem (attribute + "    0x04     IEC61937-6 MPEG-2", 5);
			itemCombo->addItem (attribute + "    0x05     IEC61937-7 ATRAC", 6);
			itemCombo->addItem (attribute + "    0x06     MBLA raw", 7);
			itemCombo->addItem (attribute + "    0x07     MBLA DVD-Audio", 8);
			itemCombo->addItem (attribute + "    0x0C     HP MBLA", 13);
			itemCombo->addItem (attribute + "    0x0D     MIDI", 14);
			itemCombo->addListener(this);
			value = value.substring(2);
			int selVal = value.getHexValue32();
			if ( (selVal>=0) && (selVal <=13) )
			{
			    selVal += 1;
			}
		
			itemCombo->setSelectedId (selVal, true);

			return itemCombo;
		}
		else if (attribute.startsWith("port_type"))
		{
			itemCombo = new ComboBox (attribute);
			itemCombo->setTooltip("click to select a setting");
			itemCombo->setEditableText (false);
			itemCombo->setJustificationType (Justification::left);
			itemCombo->setColour(ComboBox::backgroundColourId, Colours::darkseagreen);
			itemCombo->addItem (attribute + "    0x00     Speaker", 1);
			itemCombo->addItem (attribute + "    0x01     Headphone", 2);
			itemCombo->addItem (attribute + "    0x02     Microphone", 3);
			itemCombo->addItem (attribute + "    0x03     Line", 4);
			itemCombo->addItem (attribute + "    0x04     SPDIF", 5);
			itemCombo->addItem (attribute + "    0x05     ADAT", 6);
			itemCombo->addItem (attribute + "    0x06     TDIF", 7);
			itemCombo->addItem (attribute + "    0x07     MADI", 8);
			itemCombo->addItem (attribute + "    0x08     Analog", 9);
			itemCombo->addItem (attribute + "    0x09     Digital", 10);
			itemCombo->addItem (attribute + "    0x0A     MIDI", 11);
			itemCombo->addItem (attribute + "    0x0B     AES/EBU", 12);
			itemCombo->addListener(this);
			value = value.substring(2);
			itemCombo->setSelectedId (value.getHexValue32() + 1, true);
			return itemCombo;
		}
		else if (attribute.startsWith("plug_type"))
		{
			itemCombo = new ComboBox (attribute);
			itemCombo->setTooltip("click to select a setting");
			itemCombo->setEditableText (false);
			itemCombo->setJustificationType (Justification::left);
			itemCombo->setColour(ComboBox::backgroundColourId, Colours::darkseagreen);
			itemCombo->addItem (attribute + "    0x00     IsoStream", 1);
			itemCombo->addItem (attribute + "    0x01     AsyncStream", 2);
			itemCombo->addItem (attribute + "    0x02     Midi", 3);
			itemCombo->addItem (attribute + "    0x03     Sync", 4);
			itemCombo->addItem (attribute + "    0x04     Analog", 5);
			itemCombo->addItem (attribute + "    0x05     Digital", 6);
			itemCombo->addListener(this);
			value = value.substring(2);
			itemCombo->setSelectedId (value.getHexValue32() + 1, true);
			return itemCombo;
		}
		else if ( attribute.startsWith("stream_location") 
		        || attribute.startsWith("src_stream_location")
                || attribute.startsWith("dst_stream_location") )
 			{
			itemCombo = new ComboBox (attribute);
			itemCombo->setTooltip("click to select a setting");
			itemCombo->setEditableText (false);
			itemCombo->setJustificationType (Justification::left);
			itemCombo->setColour(ComboBox::backgroundColourId, Colours::darkseagreen);
			itemCombo->addItem (attribute + "    0x01     Left Front", 1);
			itemCombo->addItem (attribute + "    0x02     Right Front", 2);
			itemCombo->addItem (attribute + "    0x03     Center Front", 3);
			itemCombo->addItem (attribute + "    0x04     Low Frequency Enhancement", 4);
			itemCombo->addItem (attribute + "    0x05     Left Surround", 5);
			itemCombo->addItem (attribute + "    0x06     Right Surround", 6);
			itemCombo->addItem (attribute + "    0x07     Left of Center", 7);
			itemCombo->addItem (attribute + "    0x08     Right of Center", 8);
			itemCombo->addItem (attribute + "    0x09     Surround", 9);
			itemCombo->addItem (attribute + "    0x0A     Side Left", 10);
			itemCombo->addItem (attribute + "    0x0B     Side Right", 11);
			itemCombo->addItem (attribute + "    0x0C     Top", 12);
			itemCombo->addItem (attribute + "    0x0D     Bottom", 13);
			itemCombo->addItem (attribute + "    0x0E     Left Front Effect", 14);
			itemCombo->addItem (attribute + "    0x0F     Right Front Effect", 15);
			itemCombo->addListener(this);
			value = value.substring(2);
			itemCombo->setSelectedId (value.getHexValue32(), true);
			return itemCombo;
		}
		else if (attribute.startsWith("routing_support"))
		{
			itemCombo = new ComboBox (attribute);
			itemCombo->setTooltip("click to select a setting");
			itemCombo->setEditableText (false);
			itemCombo->setJustificationType (Justification::left);
			itemCombo->setColour(ComboBox::backgroundColourId, Colours::darkseagreen);
			itemCombo->addItem (attribute + "    0x00     Fixed", 1);
			itemCombo->addItem (attribute + "    0x01     Stays with cluster", 2);
			itemCombo->addItem (attribute + "    0x02     Flexible", 3);
			itemCombo->addListener(this);
			value = value.substring(2);
			itemCombo->setSelectedId (value.getHexValue32() + 1, true);
			return itemCombo;
		}
		else if (attribute.startsWith("signal_format"))
		{
			itemCombo = new ComboBox (attribute);
			itemCombo->setTooltip("click to select a setting");
			itemCombo->setEditableText (false);
			itemCombo->setJustificationType (Justification::left);
			itemCombo->setColour(ComboBox::backgroundColourId, Colours::darkseagreen);
			itemCombo->addItem (attribute + "    0x9000   AM824 32K", 1);
			itemCombo->addItem (attribute + "    0x9001   AM824 44.1K", 2);
			itemCombo->addItem (attribute + "    0x9002   AM824 48K", 3);
			itemCombo->addItem (attribute + "    0x9003   AM824 88.2K", 4);
			itemCombo->addItem (attribute + "    0x9004   AM824 96K", 5);
			itemCombo->addItem (attribute + "    0x9005   AM824 176.4K", 6);
			itemCombo->addItem (attribute + "    0x9006   AM824 192K", 7);
			itemCombo->addListener(this);
			value = value.substring(2);
			
			itemCombo->setSelectedId (value.getHexValue32() - 0x9000 + 1, true);
			return itemCombo;
		}
		else if (attribute.startsWith("music_plug_type"))
		{
			itemCombo = new ComboBox (attribute);
			itemCombo->setTooltip("click to select a setting");
			itemCombo->setEditableText (false);
			itemCombo->setJustificationType (Justification::left);
			itemCombo->setColour(ComboBox::backgroundColourId, Colours::darkseagreen);
			itemCombo->addItem (attribute + "    0x00   Audio", 1);
			itemCombo->addItem (attribute + "    0x01   MIDI", 2);
			itemCombo->addItem (attribute + "    0x02   SMPTE time code", 3);
			itemCombo->addItem (attribute + "    0x03   Sample Count", 4);
			itemCombo->addItem (attribute + "    0x80   Audio SYNC", 5);
			itemCombo->addListener(this);
			value = value.substring(2);
			int selVal = value.getHexValue32();
			if ((selVal>=0) && (selVal<=3))
			{
			    selVal += 1;
			}
			else if (selVal == 0x80)
			{
			    selVal = 5;
			}
			itemCombo->setSelectedId (selVal, true);
			return itemCombo;
		}
	    else
	    {
            textEditor = new TextEditor();
		    if (attribute.startsWith("raw_text_data"))
		    {
                textEditor->setTooltip("click to edit the text data");
                textEditor->setMultiLine(false);
                textEditor->setPopupMenuEnabled(true);
                textEditor->setReadOnly(false);
                textEditor->setCaretVisible(true);
                textEditor->setColour( TextEditor::backgroundColourId, Colours::darkseagreen );
                textEditor->setColour( TextEditor::highlightColourId, Colours::aliceblue);
			    textEditor->setText( attribute + "=" + value, true );
                textEditor->addListener(this);
		    }
		    else
		    {
                textEditor->setMultiLine(false);
                textEditor->setPopupMenuEnabled(false);
                textEditor->setReadOnly(true);
                textEditor->setCaretVisible(false);
                textEditor->setColour( TextEditor::focusedOutlineColourId, Colours::orange);
                textEditor->setColour( TextEditor::highlightColourId, Colours::transparentBlack);
			    textEditor->setText( attribute + "=" + value, true );
//                textEditor->addListener(this);
		    }

            return textEditor;
        }
    }
    else // an element
    {
		String xmlStr;
		
        setLinesDrawnForSubItems (true);
		xmlStr = xml->getTagName();
		
        textEditor = new TextEditor();
        textEditor->setMultiLine(false);
        textEditor->setReadOnly(true);
        textEditor->setCaretVisible(false);
        textEditor->setColour( TextEditor::backgroundColourId, Colours::silver );
        textEditor->setColour( TextEditor::focusedOutlineColourId, Colours::orange);
        textEditor->setColour( TextEditor::highlightColourId, Colours::transparentBlack);
        textEditor->setColour( TextEditor::outlineColourId, Colours::black );
        textEditor->setPopupMenuEnabled(false);
        textEditor->addMouseListener(this, false);
        
        textEditor->setText( xmlStr );

        if (xmlStr.startsWith("routing_status_ib"))
        {
#if JUCE_MAC
            textEditor->setTooltip("control-click to add sub info_blocks");
#else
			textEditor->setTooltip("right-click to add sub info_blocks");
#endif
            textEditor->setColour( TextEditor::backgroundColourId, Colours::seagreen );
            Font font = textEditor->getFont();
            font.setBold(true);
            font.setItalic(true);
            textEditor->setColour( TextEditor::textColourId, Colours::white );
            textEditor->applyFontToAllText(font);
            textEditor->addMouseListener(this, true);
            
    	    itemMenu.clear();
            itemMenu.addSectionHeader(xmlStr);
            itemMenu.addSeparator();
		    itemMenu.addItem (1, "Add su_dst_plug_ib");
		    itemMenu.addItem (2, "Add su_src_plug_ib");
		    itemMenu.addItem (3, "Add music_plug_ib");
        }
        else if ( xmlStr.startsWith("su_dst_plug_ib") || xmlStr.startsWith("su_src_plug_ib") )
        {
            String idStr = xml->getStringAttribute("su_plug_id");
            textEditor->setText(xmlStr + " (su plug " + idStr + ")", false);
#if JUCE_MAC
            textEditor->setTooltip("control-click to delete/undelete this block, or add sub info_blocks");
#else
            textEditor->setTooltip("right-click to delete/undelete this block, or add sub info_blocks");
#endif
            textEditor->setColour( TextEditor::backgroundColourId, Colours::seagreen );
            Font font = textEditor->getFont();
            font.setBold(true);
            font.setItalic(true);
            textEditor->setColour( TextEditor::textColourId, Colours::white );
            textEditor->applyFontToAllText(font);
            textEditor->addMouseListener(this, true);

    	    itemMenu.clear();
            itemMenu.addSectionHeader(xmlStr);
            itemMenu.addSeparator();
		    itemMenu.addItem (1, "Add cluster_ib");
            itemMenu.addSeparator();
		    itemMenu.addItem (100, "Delete");
        }
        else if ( xmlStr.startsWith("cluster_ib") )
        {
#if JUCE_MAC
            textEditor->setTooltip("control-click to delete/undelete this block, or add sub info_blocks");
#else
            textEditor->setTooltip("right-click to delete/undelete this block, or add sub info_blocks");
#endif
            textEditor->setColour( TextEditor::backgroundColourId, Colours::seagreen );
            Font font = textEditor->getFont();
            font.setBold(true);
            font.setItalic(true);
            textEditor->setColour( TextEditor::textColourId, Colours::white );
            textEditor->applyFontToAllText(font);
            textEditor->addMouseListener(this, true);

    	    itemMenu.clear();
            itemMenu.addSectionHeader(xmlStr);
            itemMenu.addSeparator();
		    itemMenu.addItem (1, "Add signal");
            itemMenu.addSeparator();
		    itemMenu.addItem (100, "Delete");
        }
        else if ( xmlStr.startsWith("music_plug_ib") )
        {
            Font font = textEditor->getFont();
            font.setItalic(true);
            
            String idStr = xml->getStringAttribute("music_plug_id");
            textEditor->setText(xmlStr + " (music plug " + idStr + ")", false);
#if JUCE_MAC
            textEditor->setTooltip("control-click to delete/undelete this block");
#else
            textEditor->setTooltip("right-click to delete/undelete this block");
#endif
            textEditor->applyFontToAllText(font);
            textEditor->addMouseListener(this, true);

       	    itemMenu.clear();
            itemMenu.addSectionHeader(xmlStr);
            itemMenu.addSeparator();
            textEditor->setColour( TextEditor::backgroundColourId, Colours::darkseagreen);
	        itemMenu.addItem (100, "Delete");
        }
        else if (xmlStr.startsWith("name_ib") 
                || xmlStr.startsWith("signal") 
               )
        {
            Font font = textEditor->getFont();
            font.setItalic(true);
            
#if JUCE_MAC
            textEditor->setTooltip("control-click to delete/undelete this block");
#else
            textEditor->setTooltip("right-click to delete/undelete this block");
#endif
            textEditor->applyFontToAllText(font);
            textEditor->addMouseListener(this, true);

    	    itemMenu.clear();
            itemMenu.addSectionHeader(xmlStr);
            itemMenu.addSeparator();
            textEditor->setColour( TextEditor::backgroundColourId, Colours::darkseagreen);
	        itemMenu.addItem (100, "Delete");
        }        
        return textEditor;
    }
    return 0;
}

void DescriptorTreeViewItem::mouseDown(const MouseEvent& e)
{
    setSelected(true, true);
    if( attributeIndex < 0 )
	{
		if ( e.mods.isPopupMenu() )	// checks for control-click (mac), right-click (win, mac)
		{
			const int result = itemMenu.show();
			
			if (result == 0)
			    return;

            if (result == 100)  // Delete
            {
                // check here to make sure at least one exists
                String ibStr = xml->getTagName();
                int count = 0;
                XmlElement* xmlParent = rootXml->findParentElementOf(xml);
                forEachXmlChildElementWithTagName (*xmlParent, child, ibStr)
                {
                    if ( !(child->getStringAttribute("deleted") == "true") )
                    {
                        count++;
                    }
                }

                if (count <= 1)
                {
                    if (AlertWindow::showOkCancelBox (AlertWindow::WarningIcon,
                                                 "Mark for delete",
                                                 "This will mark the last " 
                                                    + ibStr 
                                                    + " instance in this\ninfo_block for delete.\n\nAt least one may be required. Do this anyway?",
                                                 "delete",
                                                 "cancel"))
                    {
                        xml->setAttribute("deleted", "true");
//                        TreeViewItem::getParentItem()->setOpen(false);
//                        TreeViewItem::getParentItem()->setOpen(true);
                        sendActionMessage("deleted");
                    }
                }
                else
                {
                    xml->setAttribute("deleted", "true");
//                    TreeViewItem::getParentItem()->setOpen(false);
//                    TreeViewItem::getParentItem()->setOpen(true);
//                    sendActionMessage("deleted");
                    
                }
                return;
            }
            
            bool bAdd = false;
			String elementName = xml->getTagName();
			XmlElement* searchXml = NULL;
			DescriptorElementComparator xmlSorter;
			
			if (elementName == "routing_status_ib")
			{
			    switch (result)
			    {
			        case 1:
				        searchXml = ib_xml->getNextElementWithTagName("su_dst_plug_ib");
				        bAdd = true;
			            break;
			        case 2:
				        searchXml = ib_xml->getNextElementWithTagName("su_src_plug_ib");
				        bAdd = true;
			            break;
                    case 3:
					    searchXml = ib_xml->getNextElementWithTagName("music_plug_ib");
				        bAdd = true;
                        break;
                    default:
                        break;
			    }
            }
            else if ( (elementName == "su_dst_plug_ib") || (elementName == "su_src_plug_ib") )
			{
			    if (result == 1)
			    {
				    searchXml = ib_xml->getNextElementWithTagName("cluster_ib");
				    bAdd = true;
			    }
			}
            else if ( elementName == "cluster_ib" )
			{
			    if (result == 1)
			    {
				    searchXml = ib_xml->getNextElementWithTagName("signal");
				    bAdd = true;
			    }
			}
			
			if (bAdd)
		    {
		    	if (searchXml)
		        {
			        XmlElement *xmlNew = new XmlElement(*searchXml);
			        xml->addChildElement(xmlNew);
			        xml->sortChildElements(xmlSorter, true);
			        xml->setAttribute("added", "true");
			        TreeViewItem::setOpen( false );
			        TreeViewItem::setOpen( true );
		        }
            }
		}
	}
}

void DescriptorTreeViewItem::sliderValueChanged (Slider* slider)
{
    xml->setAttribute(xml->getAttributeName(attributeIndex), slider->getValue());
}

void DescriptorTreeViewItem::sliderDragEnded (Slider* slider)
{
}

void DescriptorTreeViewItem::textEditorTextChanged(TextEditor & editor)
{
}

void DescriptorTreeViewItem::textEditorEscapeKeyPressed(TextEditor & editor)
{
}

void DescriptorTreeViewItem::textEditorReturnKeyPressed(TextEditor & editor)
{
    getTextEditorText(editor);
}


void DescriptorTreeViewItem::textEditorFocusLost(TextEditor & editor)
{
    getTextEditorText(editor);
}
    
var DescriptorTreeViewItem::getDragSourceDescription()
{
	return "TreeView Items";
}

void DescriptorTreeViewItem::comboBoxChanged (ComboBox* changedCombo)
{
    String nameStr = changedCombo->getName();
    String valStr = changedCombo->getItemText(changedCombo->getSelectedItemIndex());
    valStr = valStr.substring(nameStr.length());    // item text always includes the attribute name
    valStr = valStr.trimStart();                    // followed by a few spaces
    valStr = valStr.upToFirstOccurrenceOf(" ", false, true); // then the actual value is followed by a few more spaces
    xml->setAttribute(nameStr, valStr);
}

