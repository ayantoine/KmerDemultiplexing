// This provides the frozen (compiled bytecode) files that are included if
// any.
#include <Python.h>

#include "nuitka/constants_blob.h"

// Blob from which modules are unstreamed.
#define stream_data constant_bin

// These modules should be loaded as bytecode. They may e.g. have to be loadable
// during "Py_Initialize" already, or for irrelevance, they are only included
// in this un-optimized form. These are not compiled by Nuitka, and therefore
// are not accelerated at all, merely bundled with the binary or module, so
// that CPython library can start out finding them.

struct frozen_desc {
    char const *name;
    ssize_t start;
    int size;
};

void copyFrozenModulesTo( struct _frozen *destination )
{
    struct frozen_desc frozen_modules[] = {
        { "base64", 6368831, 11253 },
        { "codecs", 6380084, 36628 },
        { "copy_reg", 6416712, 5137 },
        { "encodings", 6421849, -4362 },
        { "encodings.aliases", 6426211, 8760 },
        { "encodings.ascii", 6434971, 2253 },
        { "encodings.base64_codec", 6437224, 3829 },
        { "encodings.big5", 6441053, 1748 },
        { "encodings.big5hkscs", 6442801, 1788 },
        { "encodings.bz2_codec", 6444589, 4721 },
        { "encodings.charmap", 6449310, 3465 },
        { "encodings.cp037", 6452775, 2829 },
        { "encodings.cp1006", 6455604, 2915 },
        { "encodings.cp1026", 6458519, 2843 },
        { "encodings.cp1140", 6461362, 2829 },
        { "encodings.cp1250", 6464191, 2866 },
        { "encodings.cp1251", 6467057, 2863 },
        { "encodings.cp1252", 6469920, 2866 },
        { "encodings.cp1253", 6472786, 2879 },
        { "encodings.cp1254", 6475665, 2868 },
        { "encodings.cp1255", 6478533, 2887 },
        { "encodings.cp1256", 6481420, 2865 },
        { "encodings.cp1257", 6484285, 2873 },
        { "encodings.cp1258", 6487158, 2871 },
        { "encodings.cp424", 6490029, 2859 },
        { "encodings.cp437", 6492888, 8064 },
        { "encodings.cp500", 6500952, 2829 },
        { "encodings.cp720", 6503781, 2926 },
        { "encodings.cp737", 6506707, 8292 },
        { "encodings.cp775", 6514999, 8078 },
        { "encodings.cp850", 6523077, 7811 },
        { "encodings.cp852", 6530888, 8080 },
        { "encodings.cp855", 6538968, 8261 },
        { "encodings.cp856", 6547229, 2891 },
        { "encodings.cp857", 6550120, 7801 },
        { "encodings.cp858", 6557921, 7781 },
        { "encodings.cp860", 6565702, 8047 },
        { "encodings.cp861", 6573749, 8058 },
        { "encodings.cp862", 6581807, 8193 },
        { "encodings.cp863", 6590000, 8058 },
        { "encodings.cp864", 6598058, 8189 },
        { "encodings.cp865", 6606247, 8058 },
        { "encodings.cp866", 6614305, 8293 },
        { "encodings.cp869", 6622598, 8105 },
        { "encodings.cp874", 6630703, 2957 },
        { "encodings.cp875", 6633660, 2826 },
        { "encodings.cp932", 6636486, 1756 },
        { "encodings.cp949", 6638242, 1756 },
        { "encodings.cp950", 6639998, 1756 },
        { "encodings.euc_jis_2004", 6641754, 1812 },
        { "encodings.euc_jisx0213", 6643566, 1812 },
        { "encodings.euc_jp", 6645378, 1764 },
        { "encodings.euc_kr", 6647142, 1764 },
        { "encodings.gb18030", 6648906, 1772 },
        { "encodings.gb2312", 6650678, 1764 },
        { "encodings.gbk", 6652442, 1740 },
        { "encodings.hex_codec", 6654182, 3781 },
        { "encodings.hp_roman8", 6657963, 4112 },
        { "encodings.hz", 6662075, 1732 },
        { "encodings.idna", 6663807, 6368 },
        { "encodings.iso2022_jp", 6670175, 1801 },
        { "encodings.iso2022_jp_1", 6671976, 1817 },
        { "encodings.iso2022_jp_2", 6673793, 1817 },
        { "encodings.iso2022_jp_2004", 6675610, 1841 },
        { "encodings.iso2022_jp_3", 6677451, 1817 },
        { "encodings.iso2022_jp_ext", 6679268, 1833 },
        { "encodings.iso2022_kr", 6681101, 1801 },
        { "encodings.iso8859_1", 6682902, 2868 },
        { "encodings.iso8859_10", 6685770, 2883 },
        { "encodings.iso8859_11", 6688653, 2977 },
        { "encodings.iso8859_13", 6691630, 2886 },
        { "encodings.iso8859_14", 6694516, 2904 },
        { "encodings.iso8859_15", 6697420, 2883 },
        { "encodings.iso8859_16", 6700303, 2885 },
        { "encodings.iso8859_2", 6703188, 2868 },
        { "encodings.iso8859_3", 6706056, 2875 },
        { "encodings.iso8859_4", 6708931, 2868 },
        { "encodings.iso8859_5", 6711799, 2869 },
        { "encodings.iso8859_6", 6714668, 2913 },
        { "encodings.iso8859_7", 6717581, 2876 },
        { "encodings.iso8859_8", 6720457, 2907 },
        { "encodings.iso8859_9", 6723364, 2868 },
        { "encodings.johab", 6726232, 1756 },
        { "encodings.koi8_r", 6727988, 2890 },
        { "encodings.koi8_u", 6730878, 2876 },
        { "encodings.latin_1", 6733754, 2283 },
        { "encodings.mac_arabic", 6736037, 8014 },
        { "encodings.mac_centeuro", 6744051, 2937 },
        { "encodings.mac_croatian", 6746988, 2945 },
        { "encodings.mac_cyrillic", 6749933, 2935 },
        { "encodings.mac_farsi", 6752868, 2849 },
        { "encodings.mac_greek", 6755717, 2889 },
        { "encodings.mac_iceland", 6758606, 2928 },
        { "encodings.mac_latin2", 6761534, 4907 },
        { "encodings.mac_roman", 6766441, 2906 },
        { "encodings.mac_romanian", 6769347, 2946 },
        { "encodings.mac_turkish", 6772293, 2929 },
        { "encodings.palmos", 6775222, 3066 },
        { "encodings.ptcp154", 6778288, 4890 },
        { "encodings.punycode", 6783178, 7942 },
        { "encodings.quopri_codec", 6791120, 3647 },
        { "encodings.raw_unicode_escape", 6794767, 2202 },
        { "encodings.rot_13", 6796969, 3656 },
        { "encodings.shift_jis", 6800625, 1788 },
        { "encodings.shift_jis_2004", 6802413, 1828 },
        { "encodings.shift_jisx0213", 6804241, 1828 },
        { "encodings.string_escape", 6806069, 2061 },
        { "encodings.tis_620", 6808130, 2938 },
        { "encodings.undefined", 6811068, 2589 },
        { "encodings.unicode_escape", 6813657, 2150 },
        { "encodings.unicode_internal", 6815807, 2176 },
        { "encodings.utf_16", 6817983, 5160 },
        { "encodings.utf_16_be", 6823143, 1990 },
        { "encodings.utf_16_le", 6825133, 1990 },
        { "encodings.utf_32", 6827123, 5724 },
        { "encodings.utf_32_be", 6832847, 1883 },
        { "encodings.utf_32_le", 6834730, 1883 },
        { "encodings.utf_7", 6836613, 1883 },
        { "encodings.utf_8", 6838496, 1942 },
        { "encodings.utf_8_sig", 6840438, 4977 },
        { "encodings.uu_codec", 6845415, 4909 },
        { "encodings.zlib_codec", 6850324, 4641 },
        { "functools", 6854965, 6561 },
        { "locale", 6861526, 56524 },
        { "quopri", 6918050, 6544 },
        { "re", 6924594, 13363 },
        { "sre_compile", 6937957, 12522 },
        { "sre_constants", 6950479, 6177 },
        { "sre_parse", 6956656, 21076 },
        { "string", 6977732, 20349 },
        { "stringprep", 6998081, 14439 },
        { "struct", 7012520, 229 },
        { "types", 7012749, 2703 },
        { NULL, 0, 0 }
    };

    struct frozen_desc *current = frozen_modules;

    for(;;)
    {
        destination->name = (char *)current->name;
        destination->code = (unsigned char *)&constant_bin[ current->start ];
        destination->size = current->size;

        if (destination->name == NULL) break;

        current += 1;
        destination += 1;
    };
}
