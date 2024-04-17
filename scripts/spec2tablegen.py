import argparse
import sys
import types
import yaml

try:
    from yaml import CFullLoader as Loader
except ImportError:
    from yaml import Loader

prefix = "OL"

def format_string(s):
    if '\n' in s.strip():
        return "[{{{}}}]".format(s.strip())
    else:
        return "\"{}\"".format(s.strip())

def make_let_line(entry, field_yaml, field_td = None):
    if field_yaml not in entry:
        return ""
    if not field_td:
        field_td = field_yaml
    return "  let {} = {};\n".format(field_td, format_string(entry[field_yaml]))

def process_macro(entry):
    td_out = """\
def : Macro {{
  let name = "{}";
{}}}
"""
    td_opt = ""
    td_opt += make_let_line(entry, 'desc')
    td_opt += make_let_line(entry, 'condition')
    td_opt += make_let_line(entry, 'value')
    td_opt += make_let_line(entry, 'altvalue', 'alt_value')
    print(td_out.format(entry['name'], td_opt))


def process_typedef(entry):
    td_out = """\
def : Typedef {{
  let name = "{}";
  let value = "{}";
{}}}
"""
    td_opt = make_let_line(entry, 'desc')
    print(td_out.format(entry['name'], entry['value'], td_opt))

def process_handle(entry):
    td_out = """\
def : Handle {{
{}}}
"""
    td_opt = ""
    td_opt += make_let_line(entry, 'name')
    td_opt += make_let_line(entry, 'desc')
    print(td_out.format(td_opt))

def process_enum(entry):
    td_out = """\
def : Enum {{
{}\
  let etors =[
{}
  ];
}}
"""
    td_opt = ""
    td_opt += make_let_line(entry, 'name')
    td_opt += make_let_line(entry, 'desc')
    if 'typed_etors' in entry and entry['typed_etors']:
        td_opt += "    let is_typed = 1;\n"

    # TODO: Support `value`` field
    td_etors = ""
    for etor in entry['etors']:
        td_etors += "    Etor<\"{}\", {}>,\n".format(etor['name'], format_string(etor['desc']))

    print(td_out.format(td_opt, td_etors[:-2]))

def process_struct(entry):
    td_out = """\
def : Struct {{
{}\
  let members = [
{}
  ];
}}
"""
    td_opt = ""
    td_opt += make_let_line(entry, 'name')
    td_opt += make_let_line(entry, 'desc')
    td_opt += make_let_line(entry, 'base', 'base_class')

    td_members = ""
    for member in entry['members']:
        td_members += "    StructMember<\"{}\", \"{}\", {}>,\n".format(member['type'], member['name'], format_string(member['desc']))
    print(td_out.format(td_opt, td_members[:-2]))

# This is looks fragile and isn't optimal, but it works for the whole UR spec
# and we don't care about performance, so no point making this more clever.
def process_param_desc(desc):
    if desc.startswith("[in][out][optional]"):
        return desc[19:].strip(), "!or(!or(PARAM_IN, PARAM_OUT), PARAM_OPTIONAL)"
    elif desc.startswith("[in][out]"):
        return desc[9:].strip(), "!or(PARAM_IN, PARAM_OUT)"
    elif desc.startswith("[in][optional]"):
        return desc[14:].strip(), "!or(PARAM_IN, PARAM_OPTIONAL)"
    elif desc.startswith("[out][optional]"):
        return desc[15:].strip(), "!or(PARAM_OUT, PARAM_OPTIONAL)"
    elif desc.startswith("[out]"):
        return desc[5:].strip(), "PARAM_OUT"
    elif desc.startswith("[optional]"):
        return desc[10:].strip(), "PARAM_OPTIONAL"
    elif desc.startswith("[in]"):
        return desc[4:].strip(), "PARAM_IN"
    return desc, "PARAM_IN"

def process_function(entry):
    td_out = """\
def : Function<\"{}\"> {{
{}\
  let params = [
{}
  ];
  let returns = [
{}
  ];
}}
"""
    td_opt = ""
    td_opt += make_let_line(entry, 'name')
    td_opt += make_let_line(entry, 'desc')
    td_opt += make_let_line(entry, 'analogues')
    if 'details' in entry and entry['details']:
        td_details = "  let details = [\n"
        for detail in entry['details']:
            td_details += "    \"{}\",\n".format(detail)
        td_opt += td_details[:-2] + "\n  ];\n"

    td_params = ""
    for param in entry['params']:
        param_desc, param_flags = process_param_desc(param['desc'])
        td_params += "    Param<\"{}\", \"{}\", \"{}\", {}>,\n".format(param['type'], param['name'], param_desc, param_flags)

    td_returns = ""
    for ret in entry['returns']:
        if isinstance(ret, dict):
            # Only expecting one entry here, iterate anyway
            for ret_val, conds in ret.items():
                td_cons = ""
                for cond in conds:
                    td_cons += "      \"{}\",\n".format(cond)
                td_returns += "    Return<\"{}\", [\n{}\n    ]>,\n".format(ret_val, td_cons[:-2])
        else:
            td_returns += "    Return<\"{}\">,\n".format(ret)

    print(td_out.format(entry['class'], td_opt, td_params[:-2], td_returns[:-2]))

def expand_tags(spec):
    out = spec
    if isinstance(spec, str):
        out = out.replace("$x", prefix.lower()).replace("$X", prefix.upper())
    elif isinstance(spec, dict):
        out = dict()
        for k, item in spec.items():
            expanded_key = k.replace("$X", prefix.upper())
            out[expanded_key] = expand_tags(item)
    elif isinstance(spec, list):
        out = [expand_tags(item) for item in spec]

    return out


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--yaml', type=argparse.FileType('r'), default=sys.stdin, help="YAML file to parse")
    args = parser.parse_args()

    spec = yaml.load_all(args.yaml, Loader = Loader)

    spec = expand_tags(list(spec))

    for entry in spec:
        if entry['type'] == 'header':
            pass
        elif entry['type'] == 'macro':
            process_macro(entry)
        elif entry['type'] == "typedef":
            process_typedef(entry)
        elif entry['type'] == "handle":
            process_handle(entry)
        elif entry['type'] == "enum":
            process_enum(entry)
        elif entry['type'] == "struct":
            process_struct(entry)
        elif entry['type'] == 'function':
            process_function(entry)
        else:
            print("Unsupported type: {}".format(entry['type']))
