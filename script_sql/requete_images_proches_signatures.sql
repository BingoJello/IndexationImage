SET SERVEROUPUT ON;

DECLARE
    origin_img_name     varchar2(50);
    similar_img_name    varchar2(50);
    origin_img_name     CONSTANT varchar2(50) := '23.jpg';
    max_score           float;
    score               float;
    MAX_VALUE           CONSTANT float := 10000;
    nbr_nearest         CONSTANT number := 10;
    line                multimedia%ROWTYPE;
    origin_line         multimedia%ROWTYPE;
    cursor m is SELECT * FROM multimedia;
    type tab is varray() of varchar2(10);
    nearests_img tab := tab();
    index_tab NUMBER;

BEGIN
    -- Calcul de l'image la plus proche avec les critères oracle
    index_tab := 0;

    SELECT * INTO origin_line FROM multimedia where name = origin_img_name;
    WHILE 1 LOOP
        max_score := MAX_VALUE;
        FOR line in m LOOP
            score := ABS(ordsys.ordimageSignature.evaluateScore(origin_line.signature, line.signature,
                'color = 1, texture = 0, shape =0, location = 0'));
            IF score < max_score
                   AND line.name != origin_line.name
                   AND (NOT line.name member of nearests_img OR (line.name member of nearests_img) IS NULL )
            THEN
                max_score := score;
                similar_img_name := line.name;
            END IF;
        END LOOP;
        IF max_score != MAX_VALUE THEN
            nearests_img(index_tab) := similar_img_name;
            index_tab := index_tab + 1;
        END IF;
        IF nearests_img.COUNT >= nbr_nearest OR max_score == MAX_VALUE THEN
            EXIT LOOP;
        END IF;
    END LOOP;

    DBMS_OUTPUT.PUT_LINE('Calcul des 10 images les plus proches avec les signatures oracles \n');

    IF nearests_img.COUNT == 0 THEN
        DBMS_OUTPUT.PUT_LINE('Aucune image similaire à ' || origin_line.name);
    ELSE
        FOR i in 0 .. nearests_img.COUNT LOOP
            DBMS_OUTPUT.PUT_LINE('Image(s) similaire(s) à ' || origin_line.name || ' : \n');
            DBMS_OUTPUT.PUT_LINE(nearests_img(i));
        END LOOP;
    END IF;
END;

-- Autre facon --

-- Descripteur oracle --

SELECT img1_name, img2_name, distance_similarity FROM dist_signature_img
WHERE img1_name = '23.jpg' AND distance_similarity < 5
ORDER BY(distance_similarity) ASC