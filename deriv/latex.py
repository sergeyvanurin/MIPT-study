from pylatex import Document, PageStyle, LineBreak, Section, Subsection, Math, Alignat, LargeText, Center, MediumText
from pylatex.utils import bold, italic, NoEscape
import re

class Math_LaTeX ():
    def __init__(self, doc):
        self.doc = doc

        equations = self.Read()

        self.Create(equations)


        self.doc.generate_pdf(filepath = 'Math_LaTeX', clean_tex = False)

    def Read(self):
        data = open('derived.txt', 'rt')
        equations = data.readlines()
        eqs = []
        for eq in equations:
            eqs.append(' '.join([line.strip() for line in eq.strip().splitlines()]))
        return eqs

    def Create(self, equations):
        with self.doc.create(Center()) as Centered:
            with Centered.create(Section(title='', numbering='')) as Title:
                Title.append(LargeText(bold('MATH EQUATIONS IN A PDF DOCUMENT WITH LATEX')))


        for eq in equations:
            with self.doc.create(Alignat(numbering = True, escape = False)) as math_eq:
                math_eq.append(eq)



geometry_options = {
        "head": "2.5cm",
        "left": "3cm",
        "right": "3cm",
        "bottom": "2.5cm"
        }
doc = Document(geometry_options = geometry_options, inputenc = 'utf8')
Math_LaTeX(doc)