/*
 * OV11 original TU 2: the gallery viewer captions.
 */
#include "common.h"
#include "shared.h"
#include "ov11/cajino_get.h"

#define VIEWER_TITLE_COLOR "\x0c\xbb\xff\xe5"
#define VIEWER_INFO_COLOR "\x0c\xde\xb6\xff"

struct ViewerText ViewerText[30] = {
    { {
        VIEWER_TITLE_COLOR "Shion 1",
        VIEWER_INFO_COLOR
        "Image illustration by Kunihiko Tanaka. Drawn with\n"
        "acrylic on B3 paper. The illustration shows that\n"
        "Shion's uniform is cream-colored\n"
        "with a tinge of yellow.\n",
    } },
    { {
        VIEWER_TITLE_COLOR "Shion 2",
        VIEWER_INFO_COLOR
        "Full-body design illustrations of Shion. These\n"
        "were done very early in production, so she\n"
        "doesn't have her choker (an accessory worn\n"
        "around her neck) yet.",
    } },
    { {
        VIEWER_TITLE_COLOR "Shion 3",
        VIEWER_INFO_COLOR
        "Design illustrations of Shion's facial expressions.\n"
        "Over 10 versions of Shion were drawn before\n"
        "these were selected. She had been designed\n"
        "with eyeglasses from the beginning, however.",
    } },
    { {
        VIEWER_TITLE_COLOR "chaos 1",
        VIEWER_INFO_COLOR
        "Image illustration by Kunihiko Tanaka. The lines\n"
        "accentuate the gloves, chaos' trademark.",
    } },
    { {
        VIEWER_TITLE_COLOR "chaos 2",
        VIEWER_INFO_COLOR
        "Design illustrations of chaos' facial expressions.\n"
        "The actual polygon model was based on a\n"
        "previous version of this illustration. This latter\n"
        "version emphasizes the flip of his bangs.",
    } },
    { {
        VIEWER_TITLE_COLOR "chaos 3",
        VIEWER_INFO_COLOR
        "Full-body design illustrations of chaos. As his\n"
        "character is supposed to be a slender young boy,\n"
        "his waist is drawn slim. chaos did not change\n"
        "much from the initial concept sketch.",
    } },
    { {
        VIEWER_TITLE_COLOR "Jr. 1",
        VIEWER_INFO_COLOR
        "Image illustration by Kunihiko Tanaka. He looks a\n"
        "little younger here than the design illustration.",
    } },
    { {
        VIEWER_TITLE_COLOR "Jr. 2",
        VIEWER_INFO_COLOR
        "Design illustrations of Jr.'s facial expressions.\n"
        "The requested image of Jr. was that of a rascal,\n"
        "so he is drawn here with a wider range of\n"
        "expressions than in the game.",
    } },
    { {
        VIEWER_TITLE_COLOR "Jr. 3",
        VIEWER_INFO_COLOR
        "Full-body design illustrations of Jr., without his\n"
        "long coat. His pants have zippers at the knees\n"
        "to detach the ends. Although not drawn here,\n"
        "he has twin holsters on the back of his hips.",
    } },
    { {
        VIEWER_TITLE_COLOR "Jr. 4",
        VIEWER_INFO_COLOR
        "Jr. in his long coat. He takes this coat off while\n"
        "he is on an A.G.W.S.",
    } },
    { {
        VIEWER_TITLE_COLOR "MOMO 1",
        VIEWER_INFO_COLOR
        "Image illustration by Kunihiko Tanaka. This shows\n"
        "the actual colors used in MOMO's uniform.\n"
        "Slightly different from the game, the uniform\n"
        "drawn here uses black as a base color.",
    } },
    { {
        VIEWER_TITLE_COLOR "MOMO 2",
        VIEWER_INFO_COLOR
        "Design illustrations of MOMO's facial expressions.\n"
        "The one facing forward is a rare look for MOMO,\n"
        "who in the story often looks melancholy. Her hair\n"
        "ornament design is from pre-production.",
    } },
    { {
        VIEWER_TITLE_COLOR "MOMO 3",
        VIEWER_INFO_COLOR
        "Full-body design illustrations of MOMO. The\n"
        "design request was for something that\n"
        "\"resembled a school uniform.\" It looks rather\n"
        "simple, but this costume design is intricate.",
    } },
    { {
        VIEWER_TITLE_COLOR "KOS-MOS 1",
        VIEWER_INFO_COLOR
        "Image illustration by Kunihiko Tanaka. Her body\n"
        "color in the game is more of a light blue, but\n"
        "the actual design image uses pearl white as a\n"
        "base color.",
    } },
    { {
        VIEWER_TITLE_COLOR "KOS-MOS 2",
        VIEWER_INFO_COLOR
        "Design illustrations of KOS-MOS' facial\n"
        "expressions. These were done early in production,\n"
        "as indicated by the visor on her head, which is\n"
        "different from the final design.",
    } },
    { {
        VIEWER_TITLE_COLOR "Ziggy 1",
        VIEWER_INFO_COLOR
        "Image illustration by Kunihiko Tanaka. Note that\n"
        "each image illustration is drawn with equal\n"
        "proportions to the actual, scaled down\n"
        "characters on screen.",
    } },
    { {
        VIEWER_TITLE_COLOR "Ziggy 2",
        VIEWER_INFO_COLOR
        "Design illustrations of Ziggy's facial expressions.\n"
        "His body design was not finalized yet when these\n"
        "were done, so his face is drawn slightly slimmer.",
    } },
    { {
        VIEWER_TITLE_COLOR "Ziggy 3",
        VIEWER_INFO_COLOR
        "Full-body design illustration of Ziggy. His image\n"
        "illustration was created from this picture.\n"
        "Ziggy's uniform was designed by Tadahiro Usuda.",
    } },
    { {
        VIEWER_TITLE_COLOR "Gaignun 1",
        VIEWER_INFO_COLOR
        "Design illustrations for Gaignun's facial\n"
        "expressions. Gaignun's design was developed\n"
        "in parallel with Jr.",
    } },
    { {
        VIEWER_TITLE_COLOR "Gaignun 2",
        VIEWER_INFO_COLOR
        "Full-body design illustrations for Gaignun. They\n"
        "feature his debonair style.",
    } },
    { {
        VIEWER_TITLE_COLOR "Elsa",
        VIEWER_INFO_COLOR
        "Design illustrations of the Elsa. The image of\n"
        "the Elsa is based on a combination of a swan,\n"
        "a fighter jet, and a race car. The oval parts on\n"
        "both sides contain an \"extraordinary secret.\"\n",
    } },
    { {
        VIEWER_TITLE_COLOR "AG-01",
        VIEWER_INFO_COLOR
        "Early design illustration of an A.G.W.S. by Mr. \n"
        "Ishigaki. Many versions were made before settling\n"
        "on this design. The leg joint areas required a lot\n"
        "of trial and error due to their unique form.",
    } },
    { {
        VIEWER_TITLE_COLOR "Cockpit",
        VIEWER_INFO_COLOR
        "Design illustrations of a cockpit. Although not\n"
        "seen here, the interior works like a mold to fit\n"
        "the body contours of the pilot. An EMG signal-\n"
        "sensitive gel fills the interior.\n",
    } },
    { {
        VIEWER_TITLE_COLOR "VX-9000",
        VIEWER_INFO_COLOR
        "Design illustration of an A.G.W.S. by Kouichi\n"
        "Mugitani. This design was done immediately\n"
        "following the VX-10000, and the design concept\n"
        "was to use exposed carbon fiber frames.",
    } },
    { {
        VIEWER_TITLE_COLOR "AG-04",
        VIEWER_INFO_COLOR
        "Design illustration of an A.G.W.S. by Mr. Ishigaki.\n"
        "This model was designed for hand-to-hand\n"
        "combat, with unique details around the arms and\n"
        "legs. The legs transform for optimized mobility.",
    } },
    { {
        VIEWER_TITLE_COLOR "VX-20000",
        VIEWER_INFO_COLOR
        "Design illustrations of an A.G.W.S. by Mr.\n"
        "Mugitani. The idea behind this design was the\n"
        "image of a concept car.",
    } },
    { {
        VIEWER_TITLE_COLOR "VX-4000",
        VIEWER_INFO_COLOR
        "Design illustrations of an A.G.W.S. by Mr.\n"
        "Mugitani. This was the result of a request to\n"
        "come up with a female-type A.G.W.S., and it was\n"
        "designed to be used as MOMO's machine.",
    } },
    { {
        VIEWER_TITLE_COLOR "AG-05",
        VIEWER_INFO_COLOR
        "Design illustration of an A.G.W.S. by Mr. Ishigaki.\n"
        "This heavy-armored model was designed for\n"
        "long-range fire support. The cannon mounted on\n"
        "its back is supported by claws on both arms.",
    } },
    { {
        VIEWER_TITLE_COLOR "Shion CG",
        VIEWER_INFO_COLOR
        "CG image of Shion. A high-polygon version of the\n"
        "3D model seen in the game, she was created for\n"
        "a game expo. She was presented at the expo on\n"
        "a billboard over 3m tall.",
    } },
    { {
        VIEWER_TITLE_COLOR "KOS-MOS CG",
        VIEWER_INFO_COLOR
        "CG image of KOS-MOS, also created for a game\n"
        "expo. The gun in her hand is usually stored in a\n"
        "case by her right hip.",
    } },
};

const char *Cajino_GetTitle(int index)
{
    return ViewerText[index].text[0];
}

const char *Cajino_GetInfo(int index)
{
    return ViewerText[index].text[1];
}
