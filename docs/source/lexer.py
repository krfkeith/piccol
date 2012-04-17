from pygments.lexer import RegexLexer, bygroups
from pygments.token import *

class PiccolLexer(RegexLexer):
    name = 'Piccol lexer'
    aliases = ['piccol']
    filenames = ['*.piccol']

    tokens = {
        'root': [
            (r'def|false|true|fail', Keyword),
            (r':-|->|\[|\]|\{|\}|\.|;|=|\?|\(|\)', Keyword),
            (r'[A-Z][a-zA-Z_0-9]*', Name.Function),
            (r'[a-z][a-zA-Z_0-9]*', Name),
            (r'\\[a-z][a-zA-Z_0-9]*', Name.Constant),
            (r'\\\\', Name.Constant),
            (r'#.*?$', Comment),
            (r'/\*.*?\*/', Comment),
            (r'\'', String, 'string'),
            (r'<::', String.Escape),
            (r'::>', String.Escape),
            (r'<:', String.Escape),
            (r':>', String.Escape),
            (r'<[a-zA-Z0-9]+>', Comment.Special),
            (r'.', Generic)
        ],
        'string': [
            (r'\\.', String),
            (r'\'', String, '#pop'),
            (r'.', String)
            ]
    }



def setup(sphinx):
    sphinx.add_lexer('piccol', PiccolLexer())

