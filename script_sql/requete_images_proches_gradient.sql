SET SERVEROUTPUT ON;

DECLARE
similar_img_name    varchar2(50);
    origin_img_name     CONSTANT varchar2(50) := '6.jpg';
    x                   NUMBER :=1 ;
    y                   NUMBER;
    min_score_gradient           float;
    score_gradient               float;
    MIN_VALUE           CONSTANT float := 10000;
    nbr_nearest         CONSTANT number := 10;
    line                multimedia%ROWTYPE;
    origin_line         multimedia%ROWTYPE;
cursor m is SELECT * FROM multimedia;
type tab is varray(10) of varchar2(50);
    nearests_img tab := tab();
    nearests_img_score tab := tab();
    index_tab NUMBER;


BEGIN
    index_tab := 1;

SELECT * INTO origin_line FROM multimedia where name = origin_img_name;

WHILE x = 1 LOOP
        min_score_gradient := MIN_VALUE;
FOR line in m LOOP
            score_gradient := ABS(origin_line.meangradient - line.meangradient);
            IF score_gradient < min_score_gradient AND line.name != origin_line.name THEN
                y := 0;
FOR i in 1..nearests_img.COUNT LOOP
                    y := y + 1;
                    IF line.name = nearests_img(i) THEN
                        y := -1;
                        EXIT;
END IF;
END LOOP;
                IF y = nearests_img.COUNT THEN
                     min_score_gradient := score_gradient;
                     similar_img_name := line.name;
END IF;
END IF;
END LOOP;
        IF min_score_gradient != MIN_VALUE THEN
            nearests_img.EXTEND(1);
            nearests_img_score.EXTEND(1);
            nearests_img(index_tab) := similar_img_name;
            nearests_img_score(index_tab) := min_score_gradient;
            index_tab := index_tab + 1;
END IF;
        IF nearests_img.COUNT >= nbr_nearest OR min_score_gradient = MIN_VALUE THEN
            x := 0;
END IF;
END LOOP;

    DBMS_OUTPUT.PUT_LINE('Calcul des 10 images les plus proches avec la moyenne des gradients');
    DBMS_OUTPUT.PUT_LINE('-----------------------');

    IF nearests_img.COUNT = 0 THEN
        DBMS_OUTPUT.PUT_LINE('Aucune image similaire à ' || origin_line.name);
ELSE
        DBMS_OUTPUT.PUT_LINE('Image(s) similaire(s) à ' || origin_line.name || ' : ');
        DBMS_OUTPUT.PUT_LINE('-----------------------');
FOR i in 1..nearests_img.COUNT LOOP
            DBMS_OUTPUT.PUT_LINE(nearests_img(i) || ' avec différence gradient de : ' || nearests_img_score(i));
END LOOP;
END IF;
END;