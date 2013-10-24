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

#ifndef __DESCRIPTORTREEVIEWITEM_H__
#define __DESCRIPTORTREEVIEWITEM_H__


//==============================================================================
class DescriptorTreeViewItem  : public TreeViewItem,
                                public SliderListener,
                                public TextEditorListener,
                                public ComboBoxListener,
                                public ActionBroadcaster,
								public ActionListener,
                                public MouseListener
{
    XmlElement* rootXml;
    XmlElement* xml;
    XmlElement* ib_xml;
    int attributeIndex;

    void getTextEditorText(TextEditor & editor);

public:
    TextEditor* textEditor;
	PopupMenu itemMenu;
	StringArray* choices;
    Slider* valueSlider;
    ComboBox* itemCombo;

    DescriptorTreeViewItem (XmlElement* const rootXml_, XmlElement* const xml_, XmlElement* const ib_xml_, int attributeIndex_ );
    ~DescriptorTreeViewItem();

    int getItemWidth() const;
    String getUniqueName() const;
    bool mightContainSubItems();
    void paintItem (Graphics& g, int width, int height);
    void itemOpennessChanged (bool isNowOpen);
    Component* createItemComponent( void );
    void mouseDown(const MouseEvent& e);
    void sliderValueChanged (Slider* slider);
    void sliderDragEnded (Slider* slider);
    void textEditorTextChanged(TextEditor & editor);
    void textEditorEscapeKeyPressed(TextEditor & editor);
    void textEditorReturnKeyPressed(TextEditor & editor);
    void textEditorFocusLost(TextEditor & editor);
    var getDragSourceDescription();
    void comboBoxChanged (ComboBox* changedCombo);
    void actionListenerCallback(const String &msg);
   
    class DescriptorElementComparator
    {
    public:
        static int compareElements (const XmlElement* first, const XmlElement* second) throw()
        {
            int result = 0;
            StringArray descriptorNames;
            descriptorNames.add("su_dst_plug_ib");
            descriptorNames.add("su_src_plug_ib");
            descriptorNames.add("music_plug_ib");
            descriptorNames.add("audio_ib");
            descriptorNames.add("MIDI_ib");
            descriptorNames.add("audio_SYNC_ib");
            descriptorNames.add("cluster_ib");
            descriptorNames.add("name_ib");
           
            String firstStr = first->getTagName ();
            String secondStr = second->getTagName ();

            if (descriptorNames.indexOf(firstStr) == descriptorNames.indexOf(secondStr) )
                result = 0;
            else if (descriptorNames.indexOf(firstStr) > descriptorNames.indexOf(secondStr) )
                result = 1;
            else if (descriptorNames.indexOf(firstStr) < descriptorNames.indexOf(secondStr) )
                result = -1;

            return result;
        }
    };

    juce_UseDebuggingNewOperator

};
#endif // __DESCRIPTORTREEVIEWITEM_H__
