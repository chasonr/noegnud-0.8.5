all: changelog

.PHONY: all \
	changelog.txt \
	clean spotless

changelog: changelog.txt

changelog.txt:
	git log > $@

clean:
	rm -f *.aux *.out *.log

spotless: clean
	rm -f *.tex *.pdf *.txt
