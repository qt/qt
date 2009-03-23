import sys
import os
import xml.dom.minidom

doc_cache = {}

class DraftResolution:
    # See http://www.unicode.org/cldr/process.html for description
    unconfirmed = 'unconfirmed'
    provisional = 'provisional'
    contributed = 'contributed'
    approved = 'approved'


def findChild(parent, tag_name, arg_value, draft=None):
    for node in parent.childNodes:
        if node.nodeType != node.ELEMENT_NODE:
            continue
        if node.nodeName != tag_name:
            continue
        if arg_value:
            if not node.attributes.has_key('type'):
                continue
            if node.attributes['type'].nodeValue != arg_value:
                continue
        if draft:
            if node.attributes.has_key('draft'):
                if node.attributes['draft'].nodeValue != draft:
                    continue
            elif draft != DraftResolution.approved:
                continue
        return node
    return False

def _findEntry(file, path, draft=None):
    doc = False
    if doc_cache.has_key(file):
        doc = doc_cache[file]
    else:
        doc = xml.dom.minidom.parse(file)
        doc_cache[file] = doc

    elt = doc.documentElement
    tag_spec_list = path.split("/")
    last_entry = None
    if draft is not None:
        last_entry = tag_spec_list[-1]
        tag_spec_list = tag_spec_list[:-1]
    for tag_spec in tag_spec_list:
        tag_name = tag_spec
        arg_value = ''
        left_bracket = tag_spec.find('[')
        if left_bracket != -1:
            tag_name = tag_spec[:left_bracket]
            arg_value = tag_spec[left_bracket+1:-1]
        elt = findChild(elt, tag_name, arg_value)
        if not elt:
            return ""
    if last_entry is not None:
        elt = findChild(elt, last_entry, '', draft)
        if not elt:
            return ""
    return elt.firstChild.nodeValue

def findAlias(file):
    doc = doc_cache[file]
    alias_elt = findChild(doc.documentElement, "alias", "")
    if not alias_elt:
        return False
    if not alias_elt.attributes.has_key('source'):
        return False
    return alias_elt.attributes['source'].nodeValue

def findEntry(base, path, draft=None):
    file = base + ".xml"

    if os.path.isfile(file):
        result = _findEntry(file, path, draft)
        if result:
            return result

        alias = findAlias(file)
        if alias:
            file = os.path.dirname(base) + "/" + alias + ".xml"
            if os.path.isfile(file):
                result = _findEntry(file, path, draft)
                if result:
                    return result

    file = base[:-3] + ".xml"
    if os.path.isfile(file):
        result = _findEntry(file, path, draft)
        if result:
            return result
        alias = findAlias(file)
        if alias:
            file = os.path.dirname(base) + "/" + alias + ".xml"
            if os.path.isfile(file):
                result = _findEntry(file, path, draft)
                if result:
                    return result

    if not draft:
        file = os.path.dirname(base) + "/root.xml"
        result = _findEntry(file, path, draft)
    return result

