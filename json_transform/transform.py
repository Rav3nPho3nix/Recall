import json, sys
from datetime import datetime, timedelta

def convert_old_to_new_format(old_data, reference_date=None):
    """
    Convertit l'ancien format de leçons vers le nouveau format.
    
    Args:
        old_data: Liste des leçons dans l'ancien format
        reference_date: Date de référence (datetime). Si None, utilise aujourd'hui.
    
    Returns:
        Dict avec la structure du nouveau format
    """
    if reference_date is None:
        reference_date = datetime.now()
    
    # Intervalles standard pour les révisions (en jours)
    days_intervals = [0, 1, 4, 7, 12, 35, 91, 273, 522, 1000]
    
    new_lessons = []
    next_id = 1
    
    for lesson in old_data:
        # Calculer la date de la leçon à partir du premier countdown
        # countdown[0] représente le nombre de jours jusqu'à la prochaine révision
        # Si c'est négatif, la leçon a été faite il y a countdown[0] jours
        first_countdown = lesson['countdown'][0]
        lesson_date = reference_date + timedelta(days=first_countdown)
        
        # Créer le tableau 'next' basé sur les countdowns
        # False = révision déjà faite (countdown négatif)
        # True = révision encore à faire (countdown positif)
        next_array = []
        for countdown in lesson['countdown']:
            # False si déjà fait (countdown <= 0), True si encore à faire (countdown > 0)
            next_array.append(countdown > 0)
        
        # Compléter le tableau 'next' pour avoir 10 éléments
        while len(next_array) < 10:
            next_array.append(True)
        
        new_lesson = {
            "id": next_id,
            "name": lesson['name'],
            "subject": lesson['subject'],
            "number": lesson['number'],
            "date": lesson_date.strftime('%Y-%m-%d'),
            "next": next_array[:10]  # S'assurer qu'on a exactement 10 éléments
        }
        
        new_lessons.append(new_lesson)
        next_id += 1
    
    return {
        "days": days_intervals,
        "nextId": next_id,
        "lessons": new_lessons
    }


def main():
    old_format = ""

    with open(sys.argv[1]) as f:
        old_format = json.load(f)

    # Convertir les données
    new_format = convert_old_to_new_format(old_format)
    
    # Optionnel : sauvegarder dans un fichier
    with open('lessons_after.json', 'w', encoding='utf-8') as f:
        json.dump(new_format, f, indent=3, ensure_ascii=False)
    print("\nFichier sauvegardé dans 'lessons_after.json'")


if __name__ == "__main__":
    main()