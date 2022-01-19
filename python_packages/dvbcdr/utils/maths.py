from typing import Union


def round_significant_decimals(number: Union[float, int], significant_decimals: int = 3) -> Union[float, int]:
    str_value = f"{number:.16f}"
    dot_index = str_value.find(".")

    if dot_index > 0:
        multiplier = 10 ** significant_decimals
        for i in range(dot_index + 1, len(str_value)):
            if str_value[i] == "0":
                multiplier *= 10
            else:
                break

        number = int(number * multiplier) / multiplier

    return number
