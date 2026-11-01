#include <QtGui/QColor>
#include <vector>

#include "ui/themes/Colors.h"
#include "theme/Theme.h"

namespace terbon::Colors
{
    const QColor logo_color = ttydash::Theme::primary;
    const QColor master_text_color = ttydash::Theme::highlight;
    const QColor values_color = ttydash::Theme::secondary_text;
    const QColor output_color = ttydash::Theme::text;
}
