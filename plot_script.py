import csv
import pygal
from pygal.style import Style

def read_csv_as_nested_dict(filename, keyfield, separator, quote):
    """
    Inputs:
      filename  - Name of CSV file
      keyfield  - Field to use as key for rows
      separator - Character that separates fields
      quote     - Character used to optionally quote fields
    Output:
      Returns a dictionary of dictionaries where the outer dictionary
      maps the value in the key_field to the corresponding row in the
      CSV file.  The inner dictionaries map the field names to the
      field values for that row.
    """
    info = []
    sample = 0
    with open(filename, newline='') as csvfile:

        reader = csv.DictReader(csvfile, delimiter=separator, quotechar=quote)

        for row in reader:
            info.append((sample, float(row[keyfield])))
            sample = sample + 1

    return info

#  +-------------------------------------------------------------------------+
#  |                                                                         |
#  |                                 MAIN CODE                               |
#  |                                                                         |
#  +-------------------------------------------------------------------------+

# Custom style for pygal charts
custom_style = Style(
  background='#fff',
  plot_background='#fff',
  foreground='#212529',
  foreground_strong='#212529',
  foreground_subtle='#630C0D',
  opacity='.6',
  opacity_hover='.9',
  transition='400ms ease-in',
  colors=('#F55', '#212529')
)

#  +-------------------------------------------------------------------------+
#  |                                                                         |
#  |                            DISCHARGE CHART                              |
#  |                                                                         |
#  +-------------------------------------------------------------------------+
voltage = read_csv_as_nested_dict("1_Discharge.csv", 'Bateria[mV]', ",", '"')

discharge_chart = pygal.XY(fill=True, style=custom_style,
                           title=u'Battery discharge / Descarga de la batería',
                           x_title='Samples / Muestras',
                           y_title='Voltage / Voltaje [v]',
                           show_legend=False, dots_size=2)

discharge_chart.add('Voltage [V]', voltage)

discharge_chart.render_to_file("1_Discharge.svg")

#  +-------------------------------------------------------------------------+
#  |                                                                         |
#  |                                CHARGE CHART                             |
#  |                                                                         |
#  +-------------------------------------------------------------------------+
voltage = read_csv_as_nested_dict("2_Charge.csv", 'Bateria[mV]', ",", '"')
current = read_csv_as_nested_dict("2_Charge.csv", 'Current[A]', ",", '"')

charge_chart = pygal.XY(fill=True, style=custom_style,
                           title=u'Battery charge / Carga de la batería',
                           x_title='Samples / Muestras',
                           legend_at_bottom=True, dots_size=1)

charge_chart.add('Voltage [V]', voltage)
charge_chart.add('Current [A]', current, secondary=True)

charge_chart.render_to_file("2_Charge.svg")
