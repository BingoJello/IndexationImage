declare
i       ordsys.ordimage;
j       integer;
ctx     RAW(400) := NULL;
ligne   multimedia%ROWTYPE;
cursor  m is select * from multimedia for update;
img1    multimedia%ROWTYPE;
img2    multimedia%ROWTYPE;
cursor  m_img1 is select * from multimedia;
cursor  m_img2 is select * from multimedia;


begin
    -- Init image et signature --
    for j in 1..500 loop
            update multimedia SET image = ordsys.ordimage.init(), signature = ordsys.ordimageSignature.init()
            where name = j||'.jpg';
    end loop;
    commit;

    -- Ajout image --
    for ligne in m loop
        select ligne.image into i from multimedia where name = ligne.name for update;
        i.importFrom(ctx, 'file', 'IMG', ligne.name);
        update multimedia set image = i where name = ligne.name;
    end loop;
    commit;

    -- Création signature --
    for ligne in m loop
        ligne.signature.generateSignature(ligne.image);
        update multimedia set signature = ligne.signature where current of m;
    end loop;
    commit;

    -- Création distance signature --
    for img1 in m_img1 loop
        for img2 in m_img2 loop
            insert into dist_signature_img(img1_name, img2_name, distance_similarity)
            values (img1.name, img2.name, ordsys.ordimageSignature.evaluateScore(img1.SIGNATURE, img2.SIGNATURE,
                    'color = 0.5, texture = 0, shape =0, location = 0')
            );
        end loop;
    end loop;
    commit;
end;